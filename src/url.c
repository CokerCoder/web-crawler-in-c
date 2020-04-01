#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "url.h"

// TODO:
void to_absolute(char* url) {

}

// Strip the tailing '/' for all urls
void check_ending(char* url) {
    unsigned long url_length = strlen(url);
    if (url[url_length-1] == '/') {
        url[url_length-1] = 0;
    }
}

int valid_url(char* url) {

    if (strlen(url) > 1000) {
        return 1;
    }

    // First we check if the url contains ./ or ../
    const char* seg1 = "./";
    const char* seg2 = "../";
    if (strstr(url, seg1) || strstr(url, seg2)) {
        return 1;
    }

    // Secondly check '?', '#' and '%'
    const char char1 = '?';
    const char char2 = '#';
    const char char3 = '%';
    if (strchr(url, char1) || strchr(url, char2) || strchr(url, char3)) {
        return 1;
    }

    return 0;
}

struct Url get_info(char* url) {
    struct Url info;
    sscanf(url, "http://%99[^/]%s[\n]", info.host, info.path);
    return info;
}

int parse_url(char url[]) {

    int outcome = valid_url(url);
    if (outcome == 1) {
        printf("Invalid url\n");
        return 1;
    }

    check_ending(url);

    char* text = "http://www.google.com/index/index.html";
    char host[100];
    char page[100];
    sscanf(text, "http://%99[^/]%s[\n]", host, page);
    printf("host = \"%s\"\n", host);
    printf("page = \"%s\"\n", page);

    printf("%s\n", url);

    return 0;
}
