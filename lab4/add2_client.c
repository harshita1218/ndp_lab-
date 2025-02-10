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

    while (1) {
        printf("\nOptions:\n1. Add Book\n2. Display Books\n3. Search Book\n4. Delete Book\n5. Exit\nChoose an option: ");
        int option;
        scanf("%d", &option);
        getchar();  // Clear newline

        if (option == 1) {
            printf("Enter book details (accession_number;title;author;total_pages;publisher): ");
            char details[MAX_LEN];
            fgets(details, MAX_LEN, stdin);
            snprintf(buffer, MAX_LEN, "ADD %s", details);
        } else if (option == 2) {
            strcpy(buffer, "DISPLAY");
        } else if (option == 3) {
            printf("Enter title or author to search: ");
            char keyword[100];
            fgets(keyword, 100, stdin);
            snprintf(buffer, MAX_LEN, "SEARCH %s", keyword);
        } else if (option == 4) {
            printf("Enter accession number to delete: ");
            int acc_number;
            scanf("%d", &acc_number);
            snprintf(buffer, MAX_LEN, "DELETE %d", acc_number);
        } else if (option == 5) {
            strcpy(buffer, "EXIT");
            send(sock, buffer, strlen(buffer), 0);
            break;
        }

        send(sock, buffer, strlen(buffer), 0);
        memset(buffer, 0, MAX_LEN);
        read(sock, buffer, MAX_LEN);
        printf("Server response: %s\n", buffer);
    }

    close(sock);
    return 0;
}

