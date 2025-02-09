#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Get input string from user
    printf("Enter an alphanumeric string: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';  // Remove newline character

    // Send the input string to the server
    write(sockfd, buffer, strlen(buffer));

    // Receive and print results from the server
    printf("Results from the server:\n");
    for (int i = 0; i < 2; i++) {  // Expecting two responses (one from parent and one from child)
        memset(buffer, 0, BUFFER_SIZE);
        read(sockfd, buffer, BUFFER_SIZE);
        printf("%s\n", buffer);
    }

    close(sockfd);
    return 0;
}

