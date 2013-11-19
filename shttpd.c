#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

char *response = "HTTP/1.1 200 OK\n"
                 "Server: shttpd\n"
                 "Content-Type: text/html\n\n"
                 "<h1>It works! shttpd</h1>\n";

typedef struct http_request_data http_request;
struct http_request_data {
    ssize_t len;
    char *method;
    char *path;
    char *http_version;
    char *host;
    char *body;
};

int split_lines(char *buffer, char **output, int max_size)
{
    char *tok = NULL;
    int count = 0;

    tok = strtok(buffer, "\n");
    while (tok != NULL)
    {
        if (count > max_size - 1) break;
        output[count] = tok;
        count++;
        tok = strtok(NULL, "\n");
    }
    return count;
}

http_request parse_http_request(ssize_t len, char *buffer)
{
    http_request tmp;
    int max_size = 100;
    char *lines[max_size];

    split_lines(buffer, lines, max_size);

    tmp.path = lines[0];
    tmp.len = len;
    return tmp;
}

int main(int argc, char *argv[])
{
    int socket_fd, client_socket, addr_size;
    int yes = 1;
    struct sockaddr_in server, client;
    char buffer[10000];
    ssize_t msglen;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        puts("Error creating socket");
        return -1;
    }

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
        http_request request;
        puts("Connection accepted");

        msglen = recv(client_socket, (void *) buffer, sizeof(buffer), 0);
        if (msglen < 0) {
            puts("Error recv");
        }else{
            request = parse_http_request(msglen, buffer);
            puts(request.path);
        }

        send(client_socket, response, strlen(response), 0);
        close(client_socket);
    }

    return 0;
}

