#ifndef TOOLS_H
#define TOOLS_H

/* Tool implementations return malloc'd strings (caller frees) */
char *tool_echo(const char *input);
char *tool_calc(const char *input);
char *tool_fetch(const char *input);

#endif
