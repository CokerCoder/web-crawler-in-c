#include <stdio.h>
#include <stdlib.h>

#include "url.h"
#include "socket.h"


int main(int argc , char *argv[])
{
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
    for (int i = 0; i < 100; i++) {
        visited[i] = malloc(1000 * sizeof *visited[i]);
    }
    int total = 0;

    // Get the info of starting url
    struct Url info = get_info(starting_url);

    parse_page(info.host, info.path, visited, &total);

	return 0;
}