#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

char* lookup_host (const char *host) {
	struct addrinfo hints, *res;
	int errcode;

	char* addrstr = (char*) malloc(100*sizeof(char));
	void *ptr;

	memset (&hints, 0, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	errcode = getaddrinfo (host, NULL, &hints, &res);
	if (errcode != 0)
		{
		perror ("getaddrinfo");
		}

	printf ("Host: %s\n", host);
	while (res)
		{
		inet_ntop (res->ai_family, res->ai_addr->sa_data, addrstr, 100);

		switch (res->ai_family)
			{
			case AF_INET:
			ptr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
			break;
			case AF_INET6:
			ptr = &((struct sockaddr_in6 *) res->ai_addr)->sin6_addr;
			break;
			}
		inet_ntop (res->ai_family, ptr, addrstr, 100);
		printf ("IPv%d address: %s\n", res->ai_family == PF_INET6 ? 6 : 4,
				addrstr);
		return addrstr;
		}
		
	return addrstr;
}