#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

char           usernames[1024][36];
const uint32_t PORT = 8083;

typedef struct
{
    int                socket;
    struct sockaddr_in address;
    socklen_t          addrlen;

    fd_set clients;
} server_t;

server_t init_server()
{
    server_t server;

    server.socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server.socket < 0)
    {
        perror("Socket failed!\n");
        exit(EXIT_FAILURE);
    }

    server.address.sin_family      = AF_INET;
    server.address.sin_addr.s_addr = INADDR_ANY;
    server.address.sin_port        = htons(PORT);
    server.addrlen                 = sizeof(server.address);

    FD_ZERO(&server.clients);
    FD_SET(server.socket, &server.clients);

    return server;
}

void handle_connection(server_t* server)
{
    // we have new connection
    int client_socket =
            accept(server->socket, (struct sockaddr*) &server->address, &server->addrlen);
    if (client_socket < 0)
    {
        perror("Accept error!\n");
        exit(EXIT_FAILURE);
    }

    char* hello = "Hello new client!\0";
    send(client_socket, hello, strlen(hello), 0);
    FD_SET(client_socket, &server->clients);
}

void broadcast_message(server_t server, int socket, char* buffer)
{
    for (int j = 0; j < FD_SETSIZE; j++)
    {
        if (j == server.socket || j == socket)
            continue;

        int  username_len      = strlen(usernames[socket]);
        int  final_message_len = 1024 + username_len + 2;
        char final_message[final_message_len];
        memset(final_message, 0, final_message_len);

        strcat(final_message, usernames[socket]);
        strcat(final_message, ": ");
        strcat(final_message, buffer);

        send(j, final_message, strlen(final_message), 0);
    }
}

void handle_message(server_t server, int socket)
{
    char buffer[1024] = {0};
    memset(buffer, 0, 1024);
    recv(socket, buffer, 1024, 0);

    if (strstr(buffer, "Username:") != NULL)
    {
        memcpy(usernames[socket], buffer + 10, strlen(buffer) - 10);
        return;
    }

    broadcast_message(server, socket, buffer);
}

int main()
{
    server_t server = init_server();
    if (bind(server.socket, (struct sockaddr*) &server.address, server.addrlen) < 0)
    {
        perror("Bind error!\n");
        exit(EXIT_FAILURE);
    }

    if (listen(server.socket, 3) < 0)
    {
        perror("Listen failure!\n");
        exit(EXIT_FAILURE);
    }

    printf("Server started!\n");

    while (1)
    {
        fd_set copy_fd_set = server.clients;

        if (select(FD_SETSIZE, &copy_fd_set, NULL, NULL, NULL) < 0)
        {
            perror("Select error\n");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (!FD_ISSET(i, &copy_fd_set))
                continue;

            if (i == server.socket)
            {
                handle_connection(&server);
            }
            else
            {
                handle_message(server, i);
            }
        }
    }

    close(server.socket);

    return 0;
}