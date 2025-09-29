#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "execute.h"

#define PORT 8080
#define BUFFER_SIZE 8192
#define WEBROOT "./public"

const char *get_mime_type(const char *path)
{
    const char *ext = strrchr(path, '.');
    if (!ext)
        return "application/octet-stream";

    if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0)
        return "text/html";
    if (strcmp(ext, ".css") == 0)
        return "text/css";
    if (strcmp(ext, ".js") == 0)
        return "application/javascript";
    if (strcmp(ext, ".json") == 0)
        return "application/json";
    if (strcmp(ext, ".png") == 0)
        return "image/png";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0)
        return "image/jpeg";
    if (strcmp(ext, ".gif") == 0)
        return "image/gif";
    if (strcmp(ext, ".svg") == 0)
        return "image/svg+xml";
    if (strcmp(ext, ".ico") == 0)
        return "image/x-icon";

    return "application/octet-stream";
}

void send_response(int client_fd, int status_code, const char *status_text,
                   const char *content_type, const char *body, size_t body_len)
{
    char header[BUFFER_SIZE];
    int header_len = snprintf(header, sizeof(header),
                              "HTTP/1.1 %d %s\r\n"
                              "Content-Type: %s\r\n"
                              "Content-Length: %zu\r\n"
                              "Connection: close\r\n"
                              "\r\n",
                              status_code, status_text, content_type, body_len);

    send(client_fd, header, header_len, 0);
    if (body && body_len > 0)
    {
        send(client_fd, body, body_len, 0);
    }
}

void send_file(int client_fd, const char *filepath)
{
    int fd = open(filepath, O_RDONLY);
    if (fd < 0)
    {
        const char *body = "404 Not Found";
        send_response(client_fd, 404, "Not Found", "text/plain", body, strlen(body));
        return;
    }

    struct stat file_stat;
    if (fstat(fd, &file_stat) < 0)
    {
        close(fd);
        const char *body = "500 Internal Server Error";
        send_response(client_fd, 500, "Internal Server Error", "text/plain", body, strlen(body));
        return;
    }

    const char *mime_type = get_mime_type(filepath);

    char header[BUFFER_SIZE];
    int header_len = snprintf(header, sizeof(header),
                              "HTTP/1.1 200 OK\r\n"
                              "Content-Type: %s\r\n"
                              "Content-Length: %lld\r\n"
                              "Connection: close\r\n"
                              "\r\n",
                              mime_type, (long long)file_stat.st_size);

    send(client_fd, header, header_len, 0);

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
    {
        send(client_fd, buffer, bytes_read, 0);
    }

    close(fd);
}

void handle_request(int client_fd)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received < 0)
    {
        return;
    }

    buffer[bytes_received] = '\0';

    char method[16], path[256], protocol[16];
    if (sscanf(buffer, "%s %s %s", method, path, protocol) != 3)
    {
        const char *body = "400 Bad Request";
        send_response(client_fd, 400, "Bad Request", "text/plain", body, strlen(body));
        return;
    }

    // Handle POST /execute endpoint
    if (strcmp(method, "POST") == 0 && strcmp(path, "/execute") == 0)
    {
        // Find the body of the POST request
        char *body_start = strstr(buffer, "\r\n\r\n");
        if (body_start)
        {
            body_start += 4; // Skip past the "\r\n\r\n"
            printf("POST /execute - Body: %s\n", body_start);

            // Extract temperature from JSON body (simple parsing)
            int temperature = 24; // default
            char *temp_start = strstr(body_start, "\"temperature\"");
            if (temp_start)
            {
                temp_start = strchr(temp_start, ':');
                if (temp_start)
                {
                    temp_start++;
                    temperature = atoi(temp_start);
                }
            }

            // Execute with temperature
            const char *response = execute(temperature);
            send_response(client_fd, 200, "OK", "application/json", response, strlen(response));
        }
        else
        {
            const char *error = "{\"status\":\"error\",\"message\":\"No body in request\"}";
            send_response(client_fd, 400, "Bad Request", "application/json", error, strlen(error));
        }
        return;
    }

    if (strcmp(method, "GET") != 0)
    {
        const char *body = "405 Method Not Allowed";
        send_response(client_fd, 405, "Method Not Allowed", "text/plain", body, strlen(body));
        return;
    }

    // Default to index.html if path ends with /
    if (strcmp(path, "/") == 0)
    {
        strcpy(path, "/index.html");
    }

    char filepath[512];
    snprintf(filepath, sizeof(filepath), "%s%s", WEBROOT, path);

    // Simple path traversal protection
    if (strstr(filepath, "..") != NULL)
    {
        const char *body = "403 Forbidden";
        send_response(client_fd, 403, "Forbidden", "text/plain", body, strlen(body));
        return;
    }

    printf("GET %s -> %s\n", path, filepath);
    send_file(client_fd, filepath);
}

int main()
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("Socket creation failed");
        exit(1);
    }

    // Set socket options to reuse address
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("Setsockopt failed");
        close(server_fd);
        exit(1);
    }

    // Bind socket
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_fd);
        exit(1);
    }

    // Listen
    if (listen(server_fd, 10) < 0)
    {
        perror("Listen failed");
        close(server_fd);
        exit(1);
    }

    printf("Server listening on http://localhost:%d\n", PORT);
    printf("Serving files from: %s\n\n", WEBROOT);

    // Accept connections
    while (1)
    {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0)
        {
            perror("Accept failed");
            continue;
        }

        handle_request(client_fd);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}