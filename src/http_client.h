#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

/* Simple wrappers that use libcurl.
   - http_get(url) returns a malloc'd string with the body (or NULL). Free it.
   - http_post_json_with_api_key(prompt, api_key) posts prompt as JSON to a basic LLM endpoint.
     Modify the endpoint in the implementation as needed.
*/
char *http_get(const char *url);
char *http_post_json_with_api_key(const char *prompt, const char *api_key);

#endif
