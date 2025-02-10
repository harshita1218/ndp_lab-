#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server;
    char message[1024], server_reply[1024];

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

    // Send message to the server
    if (argc > 1) {
        strcpy(message, argv[1]);  // Use command-line argument as message
    } else {
        strcpy(message, "Default message");
    }

    if (send(sock, message, strlen(message), 0) < 0) {
        perror("Send failed");
        return 1;
    }

    // Receive termination message from server
    if (recv(sock, server_reply, sizeof(server_reply), 0) > 0) {
        printf("Server reply: %s\n", server_reply);
    }

    close(sock);
    return 0;
}

