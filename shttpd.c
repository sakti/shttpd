#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

char *response = "HTTP/1.1 200 OK\n"
                 "Server: shttpd\n"
                 "Content-Type: text/html\n\n"
                 "<h1>It works! shttpd</h1>\n";

int main(int argc, char *argv[])
{
    int socket_fd, client_socket, addr_size;
    struct sockaddr_in server, client;
    char *message;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        puts("Error creating socket");
        return -1;
    }

    int yes = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        puts("Set reuse addr on socket failed");
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8000);

    if (bind(socket_fd, (struct sockaddr *) &server, sizeof(server)) < 0)
    {
        puts("Error bind port");
        return -2;
    }
    puts("Bind port done");

    listen(socket_fd, 3);
    puts("Waiting client connection");

    addr_size = sizeof(struct sockaddr_in);

    while ((client_socket = accept(socket_fd, (struct sockaddr *) &client, (socklen_t *) &addr_size)))
    {
        puts("Connection accepted");
        write(client_socket, response, strlen(response));
        close(client_socket);
    }

    return 0;
}

