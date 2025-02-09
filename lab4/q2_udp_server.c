#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_LEN 1024

// Function to search for the IP address in the database
char* search_dns(const char* domain) {
    static char response[MAX_LEN];
    FILE* file = fopen("database.txt", "r");
    if (!file) {
        perror("Failed to open database.txt");
        snprintf(response, MAX_LEN, "Error: DNS database not found.");
        return response;
    }

    char db_domain[MAX_LEN], ip_address[MAX_LEN];
    while (fscanf(file, "%s %s", db_domain, ip_address) != EOF) {
        if (strcmp(domain, db_domain) == 0) {
            snprintf(response, MAX_LEN, "Domain: %s, IP Address: %s", db_domain, ip_address);
            fclose(file);
            return response;
        }
    }

    fclose(file);
    snprintf(response, MAX_LEN, "Domain %s not found in DNS database.", domain);
    return response;
}

int main() {
    int server_sock;
    struct sockaddr_in server_addr, client_addr;
    char buffer[MAX_LEN];
    socklen_t addr_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_DGRAM, 0);  // Create UDP socket
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("UDP DNS Server listening on port %d...\n", PORT);

    while (1) {
        memset(buffer, 0, MAX_LEN);
        recvfrom(server_sock, buffer, MAX_LEN, 0, (struct sockaddr *)&client_addr, &addr_len);
        printf("Client requested domain: %s\n", buffer);

        char* response = search_dns(buffer);
        sendto(server_sock, response, strlen(response), 0, (struct sockaddr *)&client_addr, addr_len);
    }

    return 0;
}

