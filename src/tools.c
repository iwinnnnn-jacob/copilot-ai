#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tools.h"
#include "http_client.h"

/* echo: returns the input as-is */
char *tool_echo(const char *input){
    size_t n = strlen(input) + 1;
    char *r = malloc(n);
    if (!r) return NULL;
    strncpy(r, input, n);
    return r;
}

/* calc: very small parser for "a <op> b" where op is + - * / */
char *tool_calc(const char *input){
    double a, b;
    char op;
    if (sscanf(input, "%lf %c %lf", &a, &op, &b) != 3){
        char *err = malloc(64);
        snprintf(err, 64, "calc: could not parse '%s'", input);
        return err;
    }
    double res;
    if (op == '+') res = a + b;
    else if (op == '-') res = a - b;
    else if (op == '*') res = a * b;
    else if (op == '/') {
        if (b == 0.0) {
            char *err = malloc(64);
            snprintf(err, 64, "calc: division by zero");
            return err;
        }
        res = a / b;
    } else {
        char *err = malloc(64);
        snprintf(err, 64, "calc: unknown op '%c'", op);
        return err;
    }
    char *out = malloc(128);
    snprintf(out, 128, "%g", res);
    return out;
}

/* fetch: performs HTTP GET and returns the first chunk of the body */
char *tool_fetch(const char *input){
    char *body = http_get(input);
    if (!body){
        char *err = malloc(128);
        snprintf(err, 128, "fetch: failed to GET %s", input);
        return err;
    }
    /* return the first 2000 chars to avoid huge transfers */
    size_t len = strlen(body);
    size_t take = len < 2000 ? len : 2000;
    char *out = malloc(take + 1);
    memcpy(out, body, take);
    out[take] = '\0';
    free(body);
    return out;
}
