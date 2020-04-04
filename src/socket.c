#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>

#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "../lib/gumbo/gumbo.h"

#include "url.h"

#include <malloc.h>



#define MAX_BUFFER 100000
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


void parse_page(char* host, char* path, char** visited, int* total) {
    int web_socket;
    struct sockaddr_in serv_addr;

    // Get the serv_addr host from the stdin and convert it to server IP address
    struct hostent *server;

    /* Translate host name into peer's IP address ;
     * This is name translation service by the operating system
     */
    server = gethostbyname(host);

    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
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

    sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: jinyj\r\nConnection: close\r\n\r\n", path, host);

    send(web_socket, request, strlen(request), 0);
//
    char response[MAX_BUFFER];
//    printf("request: %s\n", request);
    int read = 0;
    int ptr = 0;

    while ((read = recv(web_socket, &response[ptr], sizeof response - read, 0))) {
        ptr += read;
        if (ptr >= MAX_BUFFER) {
            break;
        }
    }

//    printf("response: %s\n", response);

    close(web_socket);


    // Store all the urls this page contains
    char **urls;
    urls = malloc(100 * sizeof *urls);
    for (int i = 0; i < 100; i++) {
        urls[i] = malloc(1000 * sizeof *urls[i]);
    }
    int count = 0;

    GumboOutput *op = gumbo_parse(response);
    // Free thr response memory right away
    search_for_links(op->root, urls, &count);
    gumbo_destroy_output(&kGumboDefaultOptions, op);


    // Strip the tailing '/' again and save to list before output
    char output[1000];
    sprintf(output, "http://%s%s", host, path);
    if(output[strlen(output-1)]=='/') {
        output[strlen(output)-1] = (char) 0;
    }

    strncpy(visited[*total], output, strlen(output));

//    printf("got here 5\n");
    *total = *total + 1;
//    printf("got here 6\n");

    printf("%s\n", output);
//    printf("number of urls visited: %d\n", *total);


    // Checking the crawled urls
    int i;
    for (i = 0; i < count; i++) {
        // Check the format of the url
        if (check_url(urls[i]) == 0) {
            // Turn to absolute url
            to_abs(urls[i], host, path);
            // Check if visited before and the host components
            if (check_visited(urls[i], visited, *total) == 0 && check_components(urls[i], host) == 0) {
                // If all good, go to the url

//                printf("next visit: %s\n", urls[i]);
                struct Url info = get_info(urls[i]);
//                printf("host: %s, path: %s\n", info.host, info.path);
                parse_page(info.host, info.path, visited, total);
            }
        }
    }



    for (int j=0; j<100; j++)
    {
        free(urls[j]);
    }
    free(urls);

}