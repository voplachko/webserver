#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 8084

void parse_request(char* request, char* method, char* path, char* protocol) {
    char* token = strtok(request, " \r\n");
    if (token != NULL) {
        strncpy(method, token, strlen(token) + 1);
        token = strtok(NULL, " \r\n");
        if (token != NULL) {
            strncpy(path, token, strlen(token) + 1);
            token = strtok(NULL, " \r\n");
            if (token != NULL) {
                strncpy(protocol, token, strlen(token) + 1);
            }
        }
    }
}

void parse_query_string(char* query_string, char* name, char* age) {
    char* token = strtok(query_string, "&=");
    while (token != NULL) {
        if (strcmp(token, "name") == 0) {
            token = strtok(NULL, "&=");
            strncpy(name, token, strlen(token) + 1);
        } else if (strcmp(token, "age") == 0) {
            token = strtok(NULL, "&=");
            strncpy(age, token, strlen(token) + 1);
        }
        token = strtok(NULL, "&=");
    }
}

int main()
{
    int server_socket, client_socket;
    if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        perror("server: socket: ");
        return EXIT_FAILURE;
    }

    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        perror("server: setsockopt: ");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("server: bind: ");
        return EXIT_FAILURE;
    }

    if (listen(server_socket, 1) == -1)
    {
        perror("server: listen: ");
        return EXIT_FAILURE;
    }

    while (1)
    {
        if ((client_socket = accept(server_socket, NULL, 0)) == -1)
        {
            perror("server: accept: ");
            return EXIT_FAILURE;
        }

        char request[1024] = "";
        read(client_socket, &request, 1024);
        write(STDOUT_FILENO, request, 1024);

        char method[32], path[256], protocol[16];
        parse_request(request, method, path, protocol);

        printf("HTTP %s request for %s using protocol %s\n", method, path, protocol);

        if (strcmp(path, "/test.jpeg") == 0) {
            int fd = open("test.jpeg", O_RDONLY);
            if (fd == -1) {
                perror("server: open: ");
                return EXIT_FAILURE;
            }

            char image_buffer[1024];
            int num_bytes_read = read(fd, image_buffer, sizeof(image_buffer));
            if (num_bytes_read == -1) {
                perror("server: read: ");
                return EXIT_FAILURE;
            } 

            close(fd);

            char resp[1024] = "";
            strcat(resp, "HTTP/1.1 200 OK\r\n");
            strcat(resp, "Connection: Сlosed\r\n");
            strcat(resp, "Content-type: image/jpeg\r\n");
            strcat(resp, "Content_length: ");
            char content_length_str;
            sprintf(&content_length_str, "%d\r\n", num_bytes_read);
            strcat(resp, &content_length_str);
            strcat(resp, "\r\n");
            write(client_socket, resp, strlen(resp));
            write(client_socket, image_buffer, num_bytes_read);
            shutdown(client_socket, SHUT_RDWR);
            close(client_socket);
        } else {
            int fd = open("index.html", O_RDONLY);
            if (fd == -1) {
                perror("server: open: ");
            }
            char resp[1024] = "";
            strcat(resp, "HTTP/1.1 200 OK\r\n");
            strcat(resp, "Connection: Сlosed\r\n");
            strcat(resp, "Content-type: text/html\r\n");
            strcat(resp, "\r\n\r\n");
            printf("%s\n", resp);
            int resp_size = strlen(resp);
            write(client_socket, resp, resp_size);
            char file[1024] = "";
            read(fd, file, 304);
            printf("%s\n", file);
            write(client_socket, file, 304);
            close(fd);
            shutdown(client_socket, SHUT_RDWR);
            close(client_socket);
        }
    }

    shutdown(server_socket, SHUT_RDWR);
    close(server_socket);
    return EXIT_SUCCESS;
}