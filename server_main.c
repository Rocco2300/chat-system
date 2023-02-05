#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

const uint32_t PORT = 8083;

int main()
{
    int valread;
    int serverfd, new_socket;
    int opt = 1;

    struct sockaddr_in address;
    int                addrlen = sizeof(address);

    char  buffer[1024] = {0};
    char* hello        = "Hello from server!";

    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd < 0)
    {
        perror("Socket failed!\n");
        exit(EXIT_FAILURE);
    }

    printf("Server socket: %d\n", serverfd);

    if (setsockopt(
                serverfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                sizeof(opt)))
    {
        perror("Option failed!\n");
        exit(EXIT_FAILURE);
    }

    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port        = htons(PORT);

    if (bind(serverfd, (struct sockaddr_in*) &address, sizeof(address)) < 0)
    {
        perror("Bind error!\n");
        exit(EXIT_FAILURE);
    }

    if (listen(serverfd, 3) < 0)
    {
        perror("Listen failure!\n");
        exit(EXIT_FAILURE);
    }

    new_socket = accept(
            serverfd, (struct sockaddr_in*) &address, (socklen_t*) &addrlen);
    if (new_socket < 0)
    {
        perror("Accept error!\n");
        exit(EXIT_FAILURE);
    }

    printf("Client socket: %d\n", new_socket);

    while (1)
    {
        valread = recv(new_socket, buffer, 1024, 0);
        if (valread)
        {
            printf("%s\n", buffer);
        }
    }

    close(serverfd);

    return 0;
}