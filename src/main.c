#include <stdio.h>
#include <stdlib.h>

#include "url.h"
#include "parse.h"


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
    char* parsed_url[100] = {};

    struct Url info = get_info(starting_url);

    printf("host: %s and path: %s\n", info.host, info.path);

    parse_page(info.host, info.path);


	return 0;
}