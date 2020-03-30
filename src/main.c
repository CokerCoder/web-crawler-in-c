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

int main(int argc , char *argv[])
{

    char *contents = "<h1>Hello, World!</h1><a href=\"unimelb.edu.au\">test</a><a href=\"http://www.google.com/\">test</a>";

    GumboOutput* op = gumbo_parse(contents);
    search_for_links(op->root);
    gumbo_destroy_output(&kGumboDefaultOptions, op);

	return 0;
}