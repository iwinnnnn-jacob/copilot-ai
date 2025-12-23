#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "http_client.h"

struct mem {
    char *data;
    size_t size;
};

static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *userdata){
    size_t total = size * nmemb;
    struct mem *m = (struct mem*)userdata;
    char *tmp = realloc(m->data, m->size + total + 1);
    if (!tmp) return 0;
    m->data = tmp;
    memcpy(m->data + m->size, ptr, total);
    m->size += total;
    m->data[m->size] = '\0';
    return total;
}

char *http_get(const char *url){
    CURL *curl = curl_easy_init();
    if (!curl) return NULL;
    struct mem m = { .data = malloc(1), .size = 0 };
    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK){
        free(m.data);
        return NULL;
    }
    return m.data;
}

/* NOTE:
   This uses OpenAI's classic completions-like POST format to illustrate.
   You should edit endpoint/JSON to match the API you will call in 2025.
*/
char *http_post_json_with_api_key(const char *prompt, const char *api_key){
    CURL *curl = curl_easy_init();
    if (!curl) return NULL;

    const char *url = "https://api.openai.com/v1/chat/completions"; /* change if needed */
    struct mem m = { .data = malloc(1), .size = 0 };
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    char auth[512];
    snprintf(auth, sizeof(auth), "Authorization: Bearer %s", api_key);
    headers = curl_slist_append(headers, auth);

    /* build a minimal JSON body. Adjust model name to your account. */
    char body[8192];
    snprintf(body, sizeof(body),
        "{\"model\":\"gpt-4o-mini\",\"messages\":[{\"role\":\"user\",\"content\":\"%s\"}],\"max_tokens\":800}",
        prompt);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m);
    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK){
        free(m.data);
        return NULL;
    }

    /* Note: response is JSON. For brevity we return raw JSON text.
       In production, parse JSON and extract the assistant content. */
    return m.data;
}
