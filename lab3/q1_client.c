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
    int sock = 0;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];
    pid_t pid;

    // Creating socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("Invalid address or Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    printf("Connected to server. PID: %d, PPID: %d\n", getpid(), getppid());

    pid = fork();  // Create child process for full-duplex communication

    if (pid == 0) {  // Child process: Handles sending messages to the server
        while (1) {
            printf("Client (child process) sending: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            send(sock, buffer, strlen(buffer), 0);
        }
    } else {  // Parent process: Handles receiving messages from the server
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_read = read(sock, buffer, BUFFER_SIZE);
            if (bytes_read > 0) {
                printf("Server: %s", buffer);
            }
        }
    }

    close(sock);
    return 0;
}

