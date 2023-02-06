#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

const uint32_t PORT = 8083;

int  running;
int  read_message;
char input_buffer[1024];

void* get_input(void* arg)
{
    running = 1;
    while (1)
    {
        if (!read_message)
        {
            printf("> ");
            fgets(input_buffer, 1024, stdin);
            read_message = 1;
        }
    }
    return NULL;
}

typedef struct
{
    int                socket;
    struct sockaddr_in server_address;

    fd_set server_status;
    int    nfds;
} client_t;

void handle_message(client_t client, pthread_t* thread)
{
    char buffer[1024] = {0};

    int ret = recv(client.socket, buffer, 1024, 0);
    if (ret)
    {
        if (!running)
        {
            printf("%s\n", buffer);
            fflush(stdout);
            pthread_create(thread, NULL, get_input, NULL);
            return;
        }

        printf("\r%s\n> ", buffer);
        fflush(stdout);
    }
}

void send_message(client_t client)
{
    if (read_message)
    {
        read_message = 0;
        size_t len   = strlen(input_buffer);

        input_buffer[len - 1] = '\0';
        send(client.socket, input_buffer, len, 0);
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        perror("Usage: chat_server <username>");
        exit(EXIT_FAILURE);
    }

    client_t  client;
    pthread_t thread;

    client.socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client.socket < 0)
    {
        perror("Socket error!\n");
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&client.server_status);
    FD_SET(client.socket, &client.server_status);
    client.nfds = client.socket;

    client.server_address.sin_family = AF_INET;
    client.server_address.sin_port   = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &client.server_address.sin_addr) <= 0)
    {
        perror("Invalid address!\n");
        exit(EXIT_FAILURE);
    }

    if (connect(client.socket, (struct sockaddr*) &client.server_address,
                sizeof(client.server_address)) < 0)
    {
        perror("Connection failed!\n");
        exit(EXIT_FAILURE);
    }

    char message[1024] = {0};
    strcat(message, "Username: ");
    strcat(message, argv[1]);
    send(client.socket, message, strlen(message), 0);

    while (1)
    {
        fd_set read_fd_set  = client.server_status;
        fd_set write_fd_set = client.server_status;
        if (select(client.nfds + 1, &read_fd_set, &write_fd_set, NULL, NULL) < 0)
        {
            perror("Select error\n");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i <= client.nfds; i++)
        {
            if (FD_ISSET(i, &read_fd_set) && i == client.socket)
            {
                handle_message(client, &thread);
            }
            else if (FD_ISSET(i, &write_fd_set) && i == client.socket)
            {
                send_message(client);
            }
        }
    }

    close(client.socket);
    return 0;
}