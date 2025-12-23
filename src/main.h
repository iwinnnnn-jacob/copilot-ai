#ifndef MAIN_H
#define MAIN_H

#include <stddef.h>
#include "agent.h"
#include "tools.h"

/*
 * Header for main-related helpers.
 *
 * This provides a small set of helpers you can use to refactor src/main.c
 * (e.g., move the main logic into run_agent so it can be tested or reused).
 *
 * None of these are required by the current main.c as distributed; they're
 * provided as a convenient, well-documented interface if you want to
 * split or reuse the main logic.
 */

/* Run the agent using argc/argv. Returns process exit code. */
int run_agent(int argc, char **argv);

/*
 * Build a goal string from argv into the provided buffer.
 * Returns pointer to out_buf on success, or NULL on failure.
 * out_buf_size must be >0.
 */
char *build_goal_from_argv(int argc, char **argv, char *out_buf, size_t out_buf_size);

/*
 * Crude extractor that tries to pull the assistant's "content" string out of
 * an LLM JSON response. The returned string is malloc'd and must be free()'d.
 * This is intentionally simple — for robust parsing use a JSON parser (cJSON).
 */
char *extract_llm_content(const char *llm_json);

#endif /* MAIN_H */
