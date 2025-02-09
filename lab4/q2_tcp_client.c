#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_LEN 1024

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[MAX_LEN] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address or address not supported\n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return -1;
    }

    printf("Enter domain name to resolve (e.g., google.com): ");
    char domain[MAX_LEN];
    scanf("%s", domain);

    send(sock, domain, strlen(domain), 0);

    read(sock, buffer, MAX_LEN);
    printf("Response from DNS server:\n%s\n", buffer);

    close(sock);
    return 0;
}

