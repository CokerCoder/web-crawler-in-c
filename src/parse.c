//
// Created by Neo on 4/04/2020.
//
#include <stdio.h>
#include <string.h>

#include <sys/socket.h>

#include <unistd.h>
#include "../lib/gumbo/gumbo.h"

#include <fcntl.h>
#include <sys/time.h>

#define MAX_BUFFER 100000


// Function to force the timeout between each recv to get the full data from the socket
// https://www.binarytides.com/receive-full-data-with-recv-socket-function-in-c/
void recv_timeout(int s , int timeout, char* response)
{
    int size_recv = 0;
    int total_size= 0;
    struct timeval begin , now;
    double timediff;

    char chunk[MAX_BUFFER];

    //make socket non blocking
    fcntl(s, F_SETFL, O_NONBLOCK);

    //beginning time
    gettimeofday(&begin , NULL);

//    while(1)
//    {
////        gettimeofday(&now , NULL);
////
////        //time elapsed in seconds
////        timediff = (double)(now.tv_sec - begin.tv_sec) + 1e-6 * (double)(now.tv_usec - begin.tv_usec);
////
////        //if you got some data, then break after timeout
////        if( total_size > 0 && timediff > timeout )
////        {
////            break;
////        }
//
////        memset(chunk , 0 , MAX_BUFFER);	//clear the variable
//        if((size_recv =  recv(s , &chunk[total_size] , sizeof chunk - size_recv , 0) ) < 0)
//        {
//            //if nothing was received then we want to wait a little before trying again, 0.1 seconds
//            break;
//        }
//        else
//        {
//            total_size += size_recv;
////            printf("%s" , chunk);
//            //reset beginning time
////            gettimeofday(&begin , NULL);
//        }
//
//    }

    int read = 0;
    int ptr = 0;
    while ((read = recv(s, &response[ptr], sizeof response - read, 0))) {
        ptr += read;
        if (ptr >= MAX_BUFFER) {
            break;
        }
    }

    strncpy(response, chunk, MAX_BUFFER);
}