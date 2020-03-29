#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>


int main(int argc , char *argv[])
{
	int web_socket;
	struct sockaddr_in serv_addr;

	if (argc < 2)
    {
        fprintf(stderr, "usage %s hostname\n", argv[0]);
        exit(0);
    }

	// Get the serv_addr hostname from the stdin and convert it to server IP address
	char* hostname = argv[1];
	struct hostent * server;
	
	/* Translate host name into peer's IP address ;
     * This is name translation service by the operating system
     */
    server = gethostbyname(argv[1]);

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
	sprintf(request, "GET / HTTP/1.1\nHost: %s\nUser-Agent: jinyj\r\n\r\n", hostname);

	char response[4096];

	send(web_socket , request , strlen(request) , 0);
	recv(web_socket, &response, sizeof(response), 0);

	printf("request from the client: %s\n", request);
	printf("response from the serv_addr: %s\n", response);

    close(web_socket);

	return 0;
}