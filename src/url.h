//
// Created by Neo on 31/03/2020.
//

#ifndef UNTITLED_URL_H
#define UNTITLED_URL_H

struct Url {
    char host[100];
    char path[100];
};

void check_ending(char* url);

int valid_url(char* url);

int check_url(char url[]);


struct Url get_info(char* url);


#endif //UNTITLED_URL_H
