#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include "../lib/gumbo/gumbo.h"
#include "url.h"

#define MAX_BUFFER 100000


/*
 * Function from Gumbo library to extract all the href links from a html text
 */
static void search_for_links(GumboNode* node, char** list, int* count) {
    if (node->type != GUMBO_NODE_ELEMENT) {
        return;
    }
    GumboAttribute* href;
    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
        // Append the url to the list and increase the size by 1
        strcpy(list[*count], href->value);
        *count = *count + 1;
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
        search_for_links((GumboNode*)(children->data[i]), list, count);
    }
}


/*
 * Main function to parse a page, done recursively if there are more pages to explore
 * The integer field "if_401" will be set to 1 if we encountered a 401 response from the previous request
 * normally this field will be set to 0
 */
void parse_page(char* host, char* path, char** visited, int* total, int if_401) {
    int web_socket;
    struct sockaddr_in serv_addr;

    // Get the serv_addr host from the stdin and convert it to server IP address
    struct hostent *server;

    /* Translate host name into peer's IP address ;
     * This is name translation service by the operating system
     */
    server = gethostbyname(host);

    if (server == NULL) {
        return;
    }

    /* Building data structures for socket */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr_list[0], (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(80);
    web_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (web_socket < 0) {
        perror("ERROR opening socket");
        exit(0);
    }

    if (connect(web_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(0);
    }

    char request[2048];
    //Send some data
    if (if_401 == 0) {
        sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: jinyj\r\nConnection: close\r\n\r\n", path, host);
    } else if (if_401 == 1) {
        sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: jinyj\r\n"
                         "Authorization: Basic amlueWo6cGFzc3dvcmQ=\r\nConnection: close\r\n\r\n", path, host);
    }
    send(web_socket, request, strlen(request), 0);


    char response[MAX_BUFFER];
    int read = 0;
    int ptr = 0;

    while ((read = recv(web_socket, &response[ptr], MAX_BUFFER, 0))) {
        ptr += read;
        if (ptr >= MAX_BUFFER) {
            break;
        }
    }
    // As soon as we get the response back, close the socket
    close(web_socket);


    // Strip the tailing '/' and save this url as visited
    char output[1000];
    sprintf(output, "http://%s%s", host, path);
    if(output[strlen(output)-1]=='/') {
        output[strlen(output)-1] = '\0';
    }
    output[strlen(output)] = '\0';

    strncpy(visited[*total], output, strlen(output));
    *total = *total + 1;
    printf("%s\n", output);

    // If the number of urls visited reached 100, then we stop
    if (*total == 100) {
        return;
    }

    // Check status codes
    char status[4];
    memcpy(status, &response[9], 3 );
    status[3] = '\0';

    if (strncmp(status, "200", 3) == 0 || strncmp(status, "404", 3) == 0 || strncmp(status, "410", 3) == 0) {
        // Continue through
    }
    if (strncmp(status, "401", 3) == 0) {
        // Re-fetch the page with the checker field on
        parse_page(host, path, visited, total, 1);
        return;
    }
    if (strncmp(status, "503", 3) == 0 || strncmp(status, "504", 3) == 0) {
        // re-fetch the page
        parse_page(host, path, visited, total, 0);
        return;
    }
    if (strncmp(status, "301", 3) == 0) {
        // Get the alternative location from the header and follow that url
        const char state[] = "Location";
        char* next_url;
        next_url = strstr(response, state);

        char new_url[1000];
        sscanf(next_url, "Location: %1000[^\n]\n", new_url);

        struct Url info = get_info(new_url);
        parse_page(info.host, info.path, visited, total, 0);
        return;
    }


    // Check the content type
    // return if it's not text/html as required
    char *result = strstr(response, "Content-Type");
    if (result) {
        char type[10];
        strncpy(type, result+14, 9);
        type[9] = '\0';

        if (strncmp(type, "text/html", 9) != 0) {
            return;
        }
    } else {
        return;
    }



    //Split header and body to check the content length
    const char splitter[] = "\r\n\r\n";
    const char length[] = "Content-Length";

    char* body;
    body = strstr(response, splitter) + 4;
    char* header;
    header = strstr(response, length);

    char content_length[10];
    if (header) {
        sscanf(header, "Content-Length: %10[^\n]\n", content_length);
    }

    int expected, actual;

    expected = atoi(content_length);
    actual = strlen(body);

    if (actual < expected) {
        return;
    }

    // Allocate the memory spaces for the urls that this page contains
    char **urls;
    urls = malloc(100 * sizeof *urls);
    for (int i = 0; i < 100; i++) {
        urls[i] = malloc(1000 * sizeof *urls[i]);
    }
    int count = 0;

    GumboOutput *op = gumbo_parse(response);
    search_for_links(op->root, urls, &count);
    gumbo_destroy_output(&kGumboDefaultOptions, op);


    // Checking the crawled urls
    int n;
    for (n = 0; n < count; n++) {
        // Check the format of the url
        if (check_url(urls[n]) == 0) {
            // Turn to absolute url
            to_abs(urls[n], host, path);
            // Check if visited before and the host components
            if (check_visited(urls[n], visited, *total) == 0 && check_components(urls[n], host) == 0) {
                // If all good, go to the url
                struct Url info = get_info(urls[n]);
                parse_page(info.host, info.path, visited, total, 0);
            }
        }
    }


    // Free the memory allocated
    for (int j=0; j<100; j++)
    {
        free(urls[j]);
    }
    free(urls);

}