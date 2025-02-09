#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_LEN 1024

int main() {
    int client_sock;
    struct sockaddr_in serv_addr;
    char buffer[MAX_LEN];
    socklen_t addr_len = sizeof(serv_addr);

    client_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    printf("Select an option:\n");
    printf("1. Registration Number\n");
    printf("2. Name of the Student\n");
    printf("3. Subject Code\n");
    printf("Enter your choice: ");
    char choice[2];
    scanf("%s", choice);

    sendto(client_sock, choice, strlen(choice), 0, (struct sockaddr *)&serv_addr, addr_len);

    recvfrom(client_sock, buffer, MAX_LEN, 0, (struct sockaddr *)&serv_addr, &addr_len);
    printf("Response from server:\n%s\n", buffer);

    close(client_sock);
    return 0;
}

