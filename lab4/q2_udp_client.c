#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_LEN 1024

int main() {
    int client_sock;
    struct sockaddr_in server_addr;
    char buffer[MAX_LEN];
    socklen_t addr_len = sizeof(server_addr);

    client_sock = socket(AF_INET, SOCK_DGRAM, 0);  // Create UDP socket
    if (client_sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    printf("Enter domain name to resolve (e.g., google.com): ");
    char domain[MAX_LEN];
    scanf("%s", domain);

    sendto(client_sock, domain, strlen(domain), 0, (struct sockaddr *)&server_addr, addr_len);

    memset(buffer, 0, MAX_LEN);
    recvfrom(client_sock, buffer, MAX_LEN, 0, (struct sockaddr *)&server_addr, &addr_len);
    printf("Response from DNS server:\n%s\n", buffer);

    close(client_sock);
    return 0;
}

