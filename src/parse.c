#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "../lib/gumbo/gumbo.h"

#include "url.h"

#include <malloc.h>

void parse_page(char* host, char* path);

static void search_for_links(GumboNode* node, char** list, int* count) {
    if (node->type != GUMBO_NODE_ELEMENT) {
        return;
    }
    GumboAttribute* href;
    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {

        // store the url to a string
        char* url = (char*)href->value;
        printf("got url: %s\n", url);
        // Append the url to the list and increase the size by 1
        strncpy(list[*count], url, strlen(url));
        *count = *count + 1;
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
        search_for_links((GumboNode*)(children->data[i]), list, count);
    }
}


void parse_page(char* host, char* path)
{
    int web_socket;
    struct sockaddr_in serv_addr;

    // Get the serv_addr host from the stdin and convert it to server IP address
    struct hostent * server;

    /* Translate host name into peer's IP address ;
     * This is name translation service by the operating system
     */
    server = gethostbyname(host);

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    /* Building data structures for socket */

    bzero((char *)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    bcopy(server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    serv_addr.sin_port = htons(80);

    web_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (web_socket < 0)
    {
        perror("ERROR opening socket");
        exit(0);
    }

    if (connect(web_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        exit(0);
    }

    //Send some data
    char request[1024];

    sprintf(request, "GET %s HTTP/1.1\nHost: %s\nUser-Agent: jinyj\r\n\r\n", path, host);

    char response[10000];

//    char* response;
//    response = (char*) malloc (100000 * sizeof(char));

    send(web_socket , request , strlen(request) , 0);
    recv(web_socket, response, 100000, 0);


    // Store all the urls this page contains
    char** urls;
    urls = malloc(100 * sizeof *urls);
    for (int i=0; i<100; i++)
    {
        urls[i] = malloc(100 * sizeof *urls[i]);
    }
    int count = 0;

    GumboOutput* op = gumbo_parse(response);
    // Free thr response memory right away
    search_for_links(op->root, urls, &count);
    gumbo_destroy_output(&kGumboDefaultOptions, op);

    printf("parsed %s%s\n", host, path);


    int i;
    for(i=0;i<count;i++) {
        if (check_url(urls[i])==0) {
            struct Url info = get_info(urls[i]);
            parse_page(info.host, info.path);
        }
    }



//    for (int j=0; j<100; j++)
//    {
//        free(urls[j]);
//    }
//    free(urls);


    close(web_socket);
}