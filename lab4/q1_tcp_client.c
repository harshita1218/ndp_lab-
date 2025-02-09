#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_LEN 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[MAX_LEN] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection failed\n");
        return -1;
    }

    printf("Select an option:\n");
    printf("1. Registration Number\n");
    printf("2. Name of the Student\n");
    printf("3. Subject Code\n");
    printf("Enter your choice: ");
    char choice[2];
    scanf("%s", choice);

    send(sock, choice, strlen(choice), 0);

    read(sock, buffer, MAX_LEN);
    printf("Response from server:\n%s\n", buffer);

    close(sock);
    return 0;
}

