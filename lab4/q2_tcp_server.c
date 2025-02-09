#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
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
    int server_fd, client_sock;
    struct sockaddr_in address;
    char buffer[MAX_LEN] = {0};
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("DNS Server listening on port %d...\n", PORT);

    while (1) {
        client_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (client_sock < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        memset(buffer, 0, MAX_LEN);
        read(client_sock, buffer, MAX_LEN);
        printf("Client requested domain: %s\n", buffer);

        char* response = search_dns(buffer);
        send(client_sock, response, strlen(response), 0);

        close(client_sock);
    }

    return 0;
}

