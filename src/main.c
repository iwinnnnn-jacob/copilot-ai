#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "agent.h"
#include "tools.h"

int main(int argc, char **argv){
    Agent agent;
    agent_init(&agent);

    /* register tools */
    static Tool echo_tool = { "echo", "Echoes input", tool_echo };
    static Tool calc_tool = { "calc", "Performs simple arithmetic: 'a + b'", tool_calc };
    static Tool fetch_tool = { "fetch", "HTTP GET", tool_fetch };

    agent_register_tool(&agent, &echo_tool);
    agent_register_tool(&agent, &calc_tool);
    agent_register_tool(&agent, &fetch_tool);

    char goal[2048];
    if (argc > 1){
        /* join argv into goal */
        goal[0] = '\0';
        for(int i=1;i<argc;i++){
            strncat(goal, argv[i], sizeof(goal)-strlen(goal)-1);
            if (i < argc-1) strncat(goal, " ", sizeof(goal)-strlen(goal)-1);
        }
    } else {
        printf("Goal: ");
        if (!fgets(goal, sizeof(goal), stdin)) return 1;
        /* remove newline */
        size_t ln = strlen(goal);
        if (ln && goal[ln-1] == '\n') goal[ln-1] = '\0';
    }

    char *plan = NULL;
    char *llm_plan = agent_plan_with_llm(goal);
    if (llm_plan){
        /* In the simple implementation, the HTTP client returns raw JSON.
           Ideally parse the JSON and extract the model's message content.
           For now, attempt to find "content" in the response and extract following text.
        */
        char *content = strstr(llm_plan, "\"content\"");
        if (content){
            /* crude extraction: find the next ':' then the next quote, then end quote */
            char *colon = strchr(content, ':');
            if (colon){
                char *first_quote = strchr(colon, '\"');
                if (first_quote){
                    /* move to after the first quote */
                    first_quote++;
                    /* find closing quote - this is fragile; for robust use a JSON parser */
                    char *end_quote = strchr(first_quote, '\"');
                    if (end_quote && end_quote > first_quote){
                        size_t len = end_quote - first_quote;
                        plan = malloc(len + 1);
                        strncpy(plan, first_quote, len);
                        plan[len] = '\0';
                    }
                }
            }
        }
        free(llm_plan);
    }

    if (!plan){
        plan = agent_local_plan(goal);
        printf("[planner] using local planner\n");
    } else {
        printf("[planner] plan from LLM (extracted)\n");
    }

    printf("Plan:\n%s\n", plan);
    char *out = NULL;
    if (agent_execute_plan(&agent, plan, &out) == 0){
        printf("Agent output:\n%s\n", out);
        free(out);
    } else {
        printf("Agent failed to execute plan\n");
    }
    free(plan);
    return 0;
}
