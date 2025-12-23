#ifndef AGENT_H
#define AGENT_H

#define MAX_TOOLS 32
#define BUF_SIZE 4096

typedef char *(*tool_fn)(const char *input);

typedef struct {
    const char *name;
    const char *description;
    tool_fn fn;
} Tool;

typedef struct {
    Tool *tools[MAX_TOOLS];
    int tool_count;
} Agent;

void agent_init(Agent *a);
int agent_register_tool(Agent *a, Tool *t);
char *agent_plan_with_llm(const char *goal); /* returns raw plan text (must free) */
char *agent_local_plan(const char *goal);     /* fallback planner (must free) */
int agent_execute_plan(Agent *a, const char *plan, char **out); /* out must be freed */

#endif
