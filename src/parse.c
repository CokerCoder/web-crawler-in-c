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

#include <malloc.h>


static void search_for_links(GumboNode* node) {
    if (node->type != GUMBO_NODE_ELEMENT) {
        return;
    }
    GumboAttribute* href;
    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
        printf("%s\n", href->value);
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
        search_for_links((GumboNode*)(children->data[i]));
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

    char response[100000];


    send(web_socket , request , strlen(request) , 0);
    recv(web_socket, response, sizeof(response), 0);


    GumboOutput* op = gumbo_parse("response");
    search_for_links(op->root);
    gumbo_destroy_output(&kGumboDefaultOptions, op);

    // Restructure the url and print to stdout
    char url[1000], res[1000];
    strcpy(url, host);
    strcpy(res, path);
    strcat(url, res);
    printf("parsed url: %s\n", url);

    close(web_socket);
}