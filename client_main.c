#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

const uint32_t PORT = 8083;

int running;
int read_message;
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

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        perror("Usage: chat_server <username>");
        exit(EXIT_FAILURE);
    }

    int sock = 0, nfds;
    struct sockaddr_in serv_addr;
    fd_set master_fd_set;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("Socket error!\n");
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&master_fd_set);
    FD_SET(sock, &master_fd_set);
    nfds = sock;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address!\n");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection failed!\n");
        exit(EXIT_FAILURE);
    }

    char message[1024] = {0};
    strcat(message, "Username: ");
    strcat(message, argv[1]);
    send(sock, message, strlen(message), 0);

    pthread_t thread;
    while (1)
    {
        fd_set read_fd_set = master_fd_set;
        fd_set write_fd_set = master_fd_set;
        if (select(nfds + 1, &read_fd_set, &write_fd_set, NULL, NULL) < 0)
        {
            perror("Select error\n");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i <= nfds; i++)
        {
            if (FD_ISSET(i, &read_fd_set) && i == sock)
            {
                char buffer[1024] = {0};
                int ret = recv(sock, buffer, 1024, 0);
                if (ret)
                {
                    if (!running)
                    {
                        printf("%s\n", buffer);
                        fflush(stdout);
                        pthread_create(&thread, NULL, get_input, NULL);
                        continue;
                    }

                    printf("\r%s\n> ", buffer);
                    fflush(stdout);
                }
            }
            else if (FD_ISSET(i, &write_fd_set) && i == sock)
            {
                if (read_message)
                {
                    size_t len = strlen(input_buffer);
                    input_buffer[len - 1] = '\0';
                    send(sock, input_buffer, len, 0);
                    read_message = 0;
                }
            }
        }
    }

    close(sock);
    return 0;
}