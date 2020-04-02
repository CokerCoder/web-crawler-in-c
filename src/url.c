#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "url.h"

// TODO:
void to_absolute(char* url) {

}


void check_ending(char* url) {
    unsigned long url_length = strlen(url);
    if (url[url_length-1] != '/') {
        strcat(url, "/");
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

int check_url(char url[]) {

    int outcome = valid_url(url);
    if (outcome == 1) {
        printf("Invalid url\n");
        return 1;
    }

    check_ending(url);

    return 0;
}
