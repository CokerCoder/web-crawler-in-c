#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <malloc.h>

#include "getip.h"

int main(int argc , char *argv[])
{
	int web_socket;
	struct sockaddr_in server;

	// Get the server hostname from the stdin and convert it to IP hostname
	char* hostname = argv[1];
	char* ip = (char*) malloc(32*sizeof(char));

	ip = lookup_host(hostname); 
	
	//Create socket
	web_socket = socket(AF_INET , SOCK_STREAM , 0);
		
	server.sin_family = AF_INET;
	server.sin_port = htons( 80 );
	inet_aton(ip, &server.sin_addr.s_addr);

	//Connect to remote server
	connect(web_socket , (struct sockaddr *)&server , sizeof(server));
	
	//Send some data
	char request[] = "GET / HTTP/1.1\r\n\r\n";
	char response[4096];

	send(web_socket , request , strlen(request) , 0);
	recv(web_socket, &response, sizeof(response), 0);

	printf("request from the client: %s\n", request);
	printf("response from the server: %s\n", response);

    close(web_socket);

	return 0;
}