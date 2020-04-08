#include <stdio.h>
#include <stdlib.h>

#include "url.h"
#include "socket.h"


int main(int argc , char *argv[])
{

    // Get the hostname from stdin
    if (argc < 2)
    {
        fprintf(stderr, "usage %s hostname\n", argv[0]);
        exit(0);
    }

    char* starting_url = argv[1];
    check_ending(starting_url);

    // List of urls that have been visited, 100 maximum
    char **visited;
    visited = malloc(100 * sizeof *visited);
    if (!visited) {
        fprintf(stderr, "could not allocate memory for visited urls\n");
        exit(0);
    }
    for (int i = 0; i < 100; i++) {
        visited[i] = malloc(1000 * sizeof *visited[i]);
    }
    int total = 0;

    // Get the info of starting url
    struct Url info = get_info(starting_url);

    // Recursively parse and follow the links
    parse_page(info.host, info.path, visited, &total, 0);

    // Free the allocated memory
    for (int j = 0; j < 100; j++) {
        free(visited[j]);
    }
    free(visited);

	return 0;
}