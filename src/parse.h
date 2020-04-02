//
// Created by Neo on 1/04/2020.
//

#ifndef UNTITLED_PARSE_H
#define UNTITLED_PARSE_H

#include "../lib/gumbo/gumbo.h"

static void search_for_links(GumboNode* node, char* list[], int* count);
void parse_page(char* host, char* path);

#endif //UNTITLED_PARSE_H
