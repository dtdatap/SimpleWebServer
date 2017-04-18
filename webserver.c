/*
AUTHOR: Abhijeet Rastogi (http://www.google.com/profiles/abhijeet.1989)

This is a very simple HTTP server. Default port is 10000 and ROOT for the server is your current working directory..

You can provide command line arguments like:- $./a.aout -p [port] -r [path]

for ex.
$./a.out -p 50000 -r /home/
to start a server at port 50000 with root directory as "/home"

$./a.out -r /home/shadyabhi
starts the server at port 10000 with ROOT as /home/shadyabhi

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>

#include "linklist.h"
#include "const.h"
#include "http.h"
#include "function.h"


#define CONNMAX 4
#define BYTES 1024

char *ROOT;
int listenfd;
void error(char *);
void startServer(int);
void respond(int);
int PORT = 9999;

static volatile int keepRunning = 1;

void intHandler()
{
	printf("\nCtrl + C catched !\n");
    keepRunning = 0;

    printf("Waiting for all sockets to be closed...\n");
    int thisPid = getpid();
    while (countChildProcess(thisPid) > 0)
	{

	}

    close(listenfd);

    exit(0);
}

int main(int argc, char* argv[])
{
	signal(SIGINT, intHandler);

    struct sockaddr_in clientaddr;
    socklen_t addrlen;
    char c;

    int slot=0;

    //Parsing the command line arguments
    while ((c = getopt (argc, argv, "p:r:")) != -1)
        switch (c)
        {
        case 'r':

            break;
        case 'p':
            PORT = atoi(optarg);
            break;
        case '?':
            fprintf(stderr,"Wrong arguments given!!!\n");
            exit(1);
        default:
            exit(1);
        }

    printf("Server started at port %s%d%s width pid=%s%d%s\n","\033[92m",PORT,"\033[0m", "\033[92m",getpid(),"\033[0m");
    // Setting all elements to -1: signifies there is no client connected
    int i;

    startServer(PORT);

    // ACCEPT connections
    while (keepRunning)
    {
        addrlen = sizeof(clientaddr);
        int clients = accept(listenfd, (struct sockaddr *) &clientaddr, &addrlen);

        if (clients < 0)
            error ("accept() error");
        else
        {
            // On success, the PID of the child process is returned in the parent,
            // and 0 is returned in the child
            if (fork() == 0)
            {
                respond(clients);
                exit(0);
            }
        }
    }

    return 0;
}

/*

prevent accept() from adding more clients
have a list of the open sockets somewhere and to wait until they are all properly closed which means:
	using shutdown() to tell the client that you will no longer work on that socket
	call read() for a while to make sure that all the client has sent in the meantime has been pulled
	then using close() to free each client socket.
THEN, you can safely close() the listening socket.
*/


//start server
void startServer(int port)
{
    char cport[5];
    snprintf(cport, 5, "%d", port);

    struct addrinfo hints, *res, *p;

    // getaddrinfo for host
    memset (&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo( NULL, cport, &hints, &res) != 0)
    {
        perror ("getaddrinfo() error");
        exit(1);
    }
    // socket and bind
    for (p = res; p!=NULL; p=p->ai_next)
    {
        listenfd = socket (p->ai_family, p->ai_socktype, 0);
        if (listenfd == -1) continue;
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) break;
    }
    if (p==NULL)
    {
        perror ("socket() or bind()");
        exit(1);
    }

    freeaddrinfo(res);

    // listen for incoming connections
    if ( listen (listenfd, 1000000) != 0 )
    {
        perror("listen() error");
        exit(1);
    }
}

//client connection
void respond(int clients)
{
    char *mesg = (char*)malloc(BUFFSIZE_DATA);
    memset(mesg, 0, BUFFSIZE_DATA);

    char *returnData;
    char* requestFile;
	int bytes;
	int fileDescriptor;

    int rcvd = recv(clients, mesg, BUFFSIZE_DATA, 0);
    if (rcvd < 0)
        fprintf(stderr,("recv() error\n"));
    else if (rcvd == 0)
        fprintf(stderr,"Client disconnected upexpectedly.\n");
    else
    {
		printf("<!-- Message begin: -->\n%s<!-- Message end -->\n", mesg);

        requestFile = getRequestFile(mesg);
        printf("Requested file: |%s|\n", requestFile);
		fileDescriptor = open(requestFile, O_RDONLY);
        if (fileDescriptor != -1)    //FILE FOUND
        {
        	returnData = (char*)malloc(BUFFSIZE_VAR);
            send(clients, HTTP_200, strlen(HTTP_200), 0);
            while ((bytes = read(fileDescriptor, returnData, BUFFSIZE_VAR)) > 0)
                write(clients, returnData, bytes);
        }
        else
            write(clients, HTTP_404, strlen(HTTP_404)); //FILE NOT FOUND
    }

    free(requestFile);
    shutdown(clients, SHUT_RDWR);         //All further send and recieve operations are DISABLED...
    close(clients);
}