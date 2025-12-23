#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "agent.h"
#include "http_client.h"
#include "tools.h"

void agent_init(Agent *a){
    a->tool_count = 0;
}

int agent_register_tool(Agent *a, Tool *t){
    if (a->tool_count >= MAX_TOOLS) return -1;
    a->tools[a->tool_count++] = t;
    return 0;
}

/* Simple helper: find tool by name (case-sensitive) */
static Tool *find_tool(Agent *a, const char *name){
    for(int i=0;i<a->tool_count;i++){
        if (strcmp(a->tools[i]->name, name) == 0) return a->tools[i];
    }
    return NULL;
}

/* agent_plan_with_llm:
   Sends a prompt to an LLM and returns plain-text plan.
   The prompt asks the model to produce a plan as lines:
     TOOL_NAME: input for tool
   Example:
     echo: Hello
     fetch: https://example.com
*/
char *agent_plan_with_llm(const char *goal){
    const char *api_key = getenv("OPENAI_API_KEY");
    if (!api_key) return NULL;

    char prompt[BUF_SIZE];
    snprintf(prompt, sizeof(prompt),
        "You are a planning assistant. Produce a short plan (1-8 steps) to achieve the goal below. "
        "Output each step as `TOOL_NAME: input` on its own line. Available tools: echo, calc, fetch.\n\nGoal: %s\n\nPlan:\n",
        goal);

    /* Simple POST to an LLM endpoint using http_client. The endpoint and headers are handled there. */
    char *resp = http_post_json_with_api_key(prompt, api_key);
    /* resp must be freed by caller. The response is raw text from the model */
    return resp;
}

/* Very simple fallback planner that makes 1-2 steps:
   - If goal contains 'https://' or 'http', suggest fetch and then echo
   - Else echo the goal
*/
char *agent_local_plan(const char *goal){
    char *plan = malloc(BUF_SIZE);
    if (!plan) return NULL;
    if (strstr(goal, "http://") || strstr(goal, "https://")){
        snprintf(plan, BUF_SIZE, "fetch: %s\necho: Parsed fetch results (agent)\n", goal);
    } else {
        snprintf(plan, BUF_SIZE, "echo: %s\n", goal);
    }
    return plan;
}

/* plan format: lines "tool: input"
   This executor splits each line, finds the tool and calls it.
   The outputs from tools are concatenated into a single string returned via out (malloc'd).
*/
int agent_execute_plan(Agent *a, const char *plan, char **out){
    if (!plan) return -1;
    char *buf = malloc(BUF_SIZE);
    if (!buf) return -1;
    buf[0] = '\0';
    char *copy = strdup(plan);
    char *line = strtok(copy, "\n");
    while(line){
        /* trim leading spaces */
        while(*line == ' ' || *line == '\t') line++;
        if (*line == '\0') { line = strtok(NULL, "\n"); continue; }
        char *colon = strchr(line, ':');
        if (!colon){
            /* unknown line format - append as note */
            strncat(buf, "[note] ", BUF_SIZE - strlen(buf) - 1);
            strncat(buf, line, BUF_SIZE - strlen(buf) - 1);
            strncat(buf, "\n", BUF_SIZE - strlen(buf) - 1);
            line = strtok(NULL, "\n");
            continue;
        }
        *colon = '\0';
        char *tool_name = line;
        char *input = colon + 1;
        while(*input == ' ') input++;

        Tool *t = find_tool(a, tool_name);
        if (!t){
            char tmp[256];
            snprintf(tmp, sizeof(tmp), "[error] unknown tool '%s'\n", tool_name);
            strncat(buf, tmp, BUF_SIZE - strlen(buf) - 1);
        } else {
            char *res = t->fn(input); /* tool returns malloc'd string or NULL */
            if (res){
                strncat(buf, res, BUF_SIZE - strlen(buf) - 1);
                strncat(buf, "\n", BUF_SIZE - strlen(buf) - 1);
                free(res);
            } else {
                char tmp[256];
                snprintf(tmp, sizeof(tmp), "[error] tool '%s' returned no result\n", tool_name);
                strncat(buf, tmp, BUF_SIZE - strlen(buf) - 1);
            }
        }
        line = strtok(NULL, "\n");
    }
    free(copy);
    *out = buf;
    return 0;
}
