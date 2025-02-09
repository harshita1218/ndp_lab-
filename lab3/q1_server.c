#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];
    int addrlen = sizeof(address);
    pid_t pid;

    // Creating socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 1) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port %d...\n", PORT);

    // Accept the connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    printf("Connection accepted. PID: %d, PPID: %d\n", getpid(), getppid());

    pid = fork();  // Create child process for full-duplex communication

    if (pid == 0) {  // Child process: Handles sending messages to the client
        while (1) {
            printf("Server (child process) sending: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            send(new_socket, buffer, strlen(buffer), 0);
        }
    } else {  // Parent process: Handles receiving messages from the client
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_read = read(new_socket, buffer, BUFFER_SIZE);
            if (bytes_read > 0) {
                printf("Client: %s", buffer);
            }
        }
    }

    close(new_socket);
    close(server_fd);
    return 0;
}

