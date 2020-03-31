#include <stdio.h>
#include <string.h>


// Always add the tailing / to the end of each url
void check_ending(char* url) {
    char final = '/';
    unsigned long url_length = strlen(url);
    if (url[url_length-1] != '/') {
        strncat(url, &final, 1);
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


int parse_url(char url[]) {

    int outcome = valid_url(url);
    if (outcome == 1) {
        printf("Invalid url\n");
        return 1;
    }

    check_ending(url);

    printf("%s\n", url);

    return 0;
}
