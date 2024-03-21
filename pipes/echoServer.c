#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/un.h>

#define BUFFSIZE 2048
#define SOCKET_SERVER_PATH "#./serverSocket"

int main(){
    struct sockaddr_un server, cliente;
    char *buff;
    int bcap = BUFFSIZE;
    buff = malloc(bcap);

    int sfd;
    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);

    
    memset(&server, 0, sizeof(server));
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SOCKET_SERVER_PATH);
    server.sun_path[0] = '\0';
    socklen_t serverSize=sizeof(server);
    
    bind(sfd, (struct sockaddr *) &server, serverSize);


    socklen_t clientSize = sizeof(cliente);
    memset(&cliente, 0, sizeof(cliente));

    ssize_t bytesRead;
    
    printf("starting to receive\n");
    bytesRead = recvfrom(sfd, buff, bcap, 0, (struct sockaddr *) &cliente, &clientSize);
    buff[bytesRead] = '\0';
    printf("Server received: %s\n", buff);

    sendto(sfd, buff, strlen(buff)+1, 0, (struct sockaddr *) &cliente, clientSize);

    close(sfd);
    remove(SOCKET_SERVER_PATH);

    free(buff);
    return 0;
}
