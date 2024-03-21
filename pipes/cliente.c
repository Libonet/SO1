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
#define SOCKET_CLIENT_PATH "#./clientSocket"

int main(){
    char *buff;
    int bcap = BUFFSIZE;
    buff = malloc(bcap);
    struct sockaddr_un server, client;

    int sfd;
    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);

    memset(&client, 0, sizeof(client));
    client.sun_family = AF_UNIX;
    strcpy(client.sun_path, SOCKET_CLIENT_PATH);
    client.sun_path[0] = '\0';
    socklen_t clientSize = sizeof(client);

    bind(sfd, (struct sockaddr *) &client, clientSize);

    memset(&server, 0, sizeof(server));
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SOCKET_SERVER_PATH);
    server.sun_path[0] = '\0';

    socklen_t serverSize = sizeof(server);

    ssize_t bytesRead;
    // read(STDIN_FILENO, buff, bcap);
    strcpy(buff, "HOLA MUNDO!!!");

    sendto(sfd, buff, strlen(buff)+1, 0, (struct sockaddr *) &server, serverSize);

    bytesRead = recvfrom(sfd, buff, bcap, 0, (struct sockaddr *) &server, &serverSize);
    buff[bytesRead] = '\0';
    printf("Cliente recibe: %s\n", buff);

    close(sfd);
    remove(SOCKET_CLIENT_PATH);

    free(buff);
    return 0;
}
