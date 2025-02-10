#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server;
    char str1[BUFFER_SIZE], str2[BUFFER_SIZE], server_response[BUFFER_SIZE];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Could not create socket");
        return 1;
    }
    printf("Socket created.\n");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connection failed");
        return 1;
    }
    printf("Connected to server.\n");

    // Get two strings from the user
    printf("Enter the first string: ");
    scanf("%s", str1);
    printf("Enter the second string: ");
    scanf("%s", str2);

    // Send the strings to the server
    char message[BUFFER_SIZE];
    snprintf(message, sizeof(message), "%s %s", str1, str2);
    if (send(sock, message, strlen(message), 0) < 0) {
        perror("Send failed");
        return 1;
    }

    // Receive response from the server
    memset(server_response, 0, sizeof(server_response));
    if (recv(sock, server_response, sizeof(server_response), 0) > 0) {
        printf("Server response: %s\n", server_response);
    }

    close(sock);
    return 0;
}
