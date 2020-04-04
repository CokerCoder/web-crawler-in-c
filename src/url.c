#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "url.h"

// TODO:
void to_absolute(char* url) {

}


// Strip all tailing '/'
void check_ending(char* url) {
    unsigned long url_length = strlen(url);
    if (url[url_length-1] == '/') {
        url[url_length-1] = (char) 0;
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
    // If no path means root '/'
    if (!strcmp(info.path, "")) {
        strcpy(info.path, "/");
    }
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

// Turn every relative url to absolute
void to_abs(char* relative, char* host, char* path) {
    char abs[1000];

    // If http://, nothing to do
    if (strlen(relative)>4 && strncmp(relative, "http://", 4)==0) {
        sprintf(abs, "%s\n", relative);
    }

    // 0. If single '/', same page
    else if (strlen(relative)==1 && relative[0] == '/') {
        sprintf(abs, "http://%s%s\n", host, path);
    }

    // 1. form of "a.html", append to the end directly
    else if (relative[0]!='/') {
        sprintf(abs, "http://%s%s%s\n", host, path, relative);
    }

    // 2. form of "/a.html", append to the host name
    else if (relative[0]=='/' && relative[1]!='/') {
        sprintf(abs, "http://%s%s", host, relative);
    }

    // 3. form of "//a.html", append to http:
    else if (relative[0]=='/' && relative[1]=='/') {
        sprintf(abs, "http:%s", relative);
    }

    strncpy(relative, abs, 100);
}

// Function to check if already visited, take only absolute urls as parameter
// return 0 if not visited, 1 if visited
int check_visited(char* abs_url, char** total_list, int total) {
    int i;
    for (i=0;i<total;i++) {
        if (strncmp(abs_url, total_list[i], strlen(abs_url)-1) == 0) {
            return 1;
        }
    }
    return 0;
}
