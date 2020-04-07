#include <stdio.h>
#include <string.h>

#include "url.h"



// Strip all tailing '/'
void check_ending(char* url) {
    unsigned long url_length = strlen(url);
    if (url[url_length-1] == '/') {
        url[url_length-1] = (char) 0;
    }
}

/*
 * Function to check if an url is valid
 * A valid url in this project is considered:
 * - has less than 1000 bytes
 * - does not contain '.' or ".." (can exist but ignored)
 * - does not contain '%', '?', or '#' (can exist but ignored)
 * return 0 if the url is valid otherwise return 1
*/
int valid_url(char* url) {

    if (strlen(url) > 1000) {
        return 1;
    }

    // This check the start and the end of a url, make sure there is no spaces
    // However, as the urls in the project given is guaranteed to be a valid one
    // this condition may only check on the test websites
    if (url[0] == ' ' || url[strlen(url)-1] == ' ') {
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


/*
 * Get the basic information: host and path from a given absolute url
 * and store them into a Url structure
 */
struct Url get_info(char* url) {
    // Initialize the structure
    struct Url info = {"", ""};
    sscanf(url, "http://%99[^/]%s[\n]", info.host, info.path);
    // If no path means root '/'
    if (!strcmp(info.path, "")) {
        strcpy(info.path, "/");
    }
    return info;
}


/*
 * This function automatically remove the tailing '/' after checking if it's a valid url
 */
int check_url(char* url) {

    int valid = valid_url(url);
    if (valid == 1) {
        return 1;
    }
    check_ending(url);
    return 0;
}

// Turn every given url to absolute based on the given part
void to_abs(char* given, char* host, char* path) {
    char abs[1000];

    // If http://, nothing to do
    if (strlen(given) > 4 && strncmp(given, "http://", 4) == 0) {
        sprintf(abs, "%s\n", given);
    }

    // 0. If single '/', same page
    else if (strlen(given) == 1 && given[0] == '/') {
        sprintf(abs, "http://%s%s\n", host, path);
    }

    // 1. form of "a.html", append to the end folder
    else if (given[0] != '/') {

        // Check if char '.' appears in the path, if appears means the path itself is a file, otherwise is a directory
        // if it is a directory, append the given path to the end directly
        // or of it is a file, then replace the file name with the new given path

        // Define type, whether 'd' stands for "directory", or 'f' stands for "file"
        char type;
        if (strchr(path, '.')) {
            type = 'f';
        } else {
            type = 'd';
        }

        if (type == 'd') {
            if (path[strlen(path) - 1] != '/') {
                sprintf(abs, "http://%s%s/%s\n", host, path, given);
            } else {
                sprintf(abs, "http://%s%s%s\n", host, path, given);
            }
        }

        else {
            // Retrieve the pointer to the last occurrence of the '/'
            char* last_dir = strrchr(path, '/');
            memcpy(last_dir+1, given, strlen(given));
            *(last_dir + strlen(given) + 1) = '\0';
            sprintf(abs, "http://%s%s\n", host, path);
        }

    }

    // 2. form of "/a.html", append to the host name
    else if (given[0] == '/' && given[1] != '/') {
        sprintf(abs, "http://%s%s", host, given);
    }

    // 3. form of "//a.html", append to http:
    else if (given[0] == '/' && given[1] == '/') {
        sprintf(abs, "http:%s", given);
    }

    strcpy(given, abs);
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

// Function to check the components of the url
// Only look at the urls that all but the first components of its host name are the same
// return 0 if valid, otherwise return 1
int check_components(char* url, char* host) {

    struct Url info;
    char check_comp[1000];
    char expected_comp[1000];

    sscanf(url, "http://%99[^/]%s[\n]", info.host, info.path);

    sscanf(host, "%*[^.].%s", check_comp);
    sscanf(info.host, "%*[^.].%s", expected_comp);


    if (strcmp(check_comp, expected_comp) == 0) {
        return 0;
    }

    return 1;
}
