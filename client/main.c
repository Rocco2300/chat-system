#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

const uint32_t PORT = 8083;

int read_message;
char input_buffer[1024];

void* get_input(void* arg)
{
    while (1)
    {
        if (!read_message)
        {
            printf("message:");
            fgets(&input_buffer, 1024, stdin);
            read_message = 1;
        }
    }
    return NULL;
}

int main()
{
    int sock = 0, valread, clientfd;
    struct sockaddr_in serv_addr;
    char* hello = "Hello from client!";
    char buffer[1024] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket error!\n");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address!\n");
        exit(EXIT_FAILURE);
    }

    clientfd = connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (clientfd < 0)
    {
        perror("Connection failed!\n");
        exit(EXIT_FAILURE);
    }

    pthread_t thread;
    pthread_create(&thread, NULL, get_input, NULL);
    while (1)
    {
        if (read_message)
        {
            size_t len = strlen(input_buffer);
            input_buffer[len - 1] = '\0';
            send(sock, input_buffer, len, 0);
            read_message = 0;
        }
    }

    close(clientfd);
    return 0;
}