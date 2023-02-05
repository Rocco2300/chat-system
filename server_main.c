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
    fd_set master_fd_set;
    int master_socket;
    int opt = 1;

    struct sockaddr_in address;
    int                addrlen = sizeof(address);

    char  buffer[1024] = {0};

    master_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (master_socket < 0)
    {
        perror("Socket failed!\n");
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&master_fd_set);
    FD_SET(master_socket, &master_fd_set);

    printf("Server socket: %d\n", master_socket);

    if (setsockopt(
                master_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                sizeof(opt)))
    {
        perror("Option failed!\n");
        exit(EXIT_FAILURE);
    }

    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port        = htons(PORT);

    if (bind(master_socket, (struct sockaddr*) &address, sizeof(address)) < 0)
    {
        perror("Bind error!\n");
        exit(EXIT_FAILURE);
    }

    if (listen(master_socket, 3) < 0)
    {
        perror("Listen failure!\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        fd_set copy_fd_set = master_fd_set;

        if (select(FD_SETSIZE, &copy_fd_set, NULL, NULL, NULL) < 0)
        {
            perror("Select error\n");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (FD_ISSET(i, &copy_fd_set))
            {
                if (i == master_socket)
                {
                    // we have new connection
                    int client_socket = accept(
                            master_socket, (struct sockaddr*) &address, (socklen_t*) &addrlen);
                    if (client_socket < 0)
                    {
                        perror("Accept error!\n");
                        exit(EXIT_FAILURE);
                    }

                    printf("Client socket: %d\n", client_socket);
                    FD_SET(client_socket, &master_fd_set);
                }
            }
        }
//
//        int ret = recv(client_socket, buffer, 1024, 0);
//        if (ret)
//        {
//            printf("%s\n", buffer);
//        }
    }

    close(master_socket);

    return 0;
}