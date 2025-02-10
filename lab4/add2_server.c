#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_LEN 1024
#define MAX_BOOKS 100

struct Book {
    int accession_number;
    char title[100];
    char author[100];
    int total_pages;
    char publisher[100];
};

struct Book database[MAX_BOOKS];
int book_count = 0;

void to_lowercase(const char* source, char* dest) {
    while (*source) {
        *dest++ = tolower(*source++);
    }
    *dest = '\0';
}

void add_book(char* data) {
    if (book_count >= MAX_BOOKS) {
        printf("Database is full. Cannot add more books.\n");
        return;
    }
    sscanf(data, "%d;%99[^;];%99[^;];%d;%99[^\n]",
           &database[book_count].accession_number,
           database[book_count].title,
           database[book_count].author,
           &database[book_count].total_pages,
           database[book_count].publisher);
    book_count++;
}

char* display_books() {
    static char response[MAX_LEN * MAX_BOOKS];
    response[0] = '\0';

    if (book_count == 0) {
        snprintf(response, MAX_LEN, "No books available.\n");
        return response;
    }

    for (int i = 0; i < book_count; i++) {
        char temp[MAX_LEN];
        snprintf(temp, MAX_LEN, "Accession Number: %d, Title: %s, Author: %s, Pages: %d, Publisher: %s\n",
                 database[i].accession_number, database[i].title, database[i].author,
                 database[i].total_pages, database[i].publisher);
        strcat(response, temp);
    }
    return response;
}

#include <ctype.h>
#include <string.h>

char* trim_whitespace(char* str) {
    char* end;
    while (isspace((unsigned char)*str)) str++;  // Trim leading spaces
    if (*str == 0) return str;                   // All spaces?

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;  // Trim trailing spaces
    *(end + 1) = '\0';
    return str;
}

char* search_book(char* keyword) {
    static char response[MAX_LEN * MAX_BOOKS];
    response[0] = '\0';

    char lower_keyword[MAX_LEN];
    to_lowercase(trim_whitespace(keyword), lower_keyword);
    int found = 0;

    for (int i = 0; i < book_count; i++) {
        char lower_title[MAX_LEN], lower_author[MAX_LEN];
        to_lowercase(database[i].title, lower_title);
        to_lowercase(database[i].author, lower_author);

        if (strstr(lower_title, lower_keyword) || strstr(lower_author, lower_keyword)) {
            found = 1;
            char temp[MAX_LEN];
            snprintf(temp, MAX_LEN, "Accession Number: %d, Title: %s, Author: %s, Pages: %d, Publisher: %s\n",
                     database[i].accession_number, database[i].title, database[i].author,
                     database[i].total_pages, database[i].publisher);
            strcat(response, temp);
        }
    }

    if (!found) {
        snprintf(response, MAX_LEN, "No book found for keyword: %s\n", keyword);
    }

    return response;
}


int delete_book(int acc_number) {
    for (int i = 0; i < book_count; i++) {
        if (database[i].accession_number == acc_number) {
            for (int j = i; j < book_count - 1; j++) {
                database[j] = database[j + 1];
            }
            book_count--;
            return 1;
        }
    }
    return 0;
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

    printf("Book Database Server listening on port %d...\n", PORT);

    while (1) {
        client_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (client_sock < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        while (1) {
            memset(buffer, 0, MAX_LEN);
            read(client_sock, buffer, MAX_LEN);

            char command[10];
            sscanf(buffer, "%s", command);

            if (strcmp(command, "ADD") == 0) {
                add_book(buffer + 4);
                send(client_sock, "Book added successfully.\n", 25, 0);
            } else if (strcmp(command, "DISPLAY") == 0) {
                char* response = display_books();
                send(client_sock, response, strlen(response), 0);
            } else if (strcmp(command, "SEARCH") == 0) {
                char* response = search_book(buffer + 7);
                send(client_sock, response, strlen(response), 0);
            } else if (strcmp(command, "DELETE") == 0) {
                int acc_number;
                sscanf(buffer + 7, "%d", &acc_number);
                if (delete_book(acc_number)) {
                    send(client_sock, "Book deleted successfully.\n", 27, 0);
                } else {
                    send(client_sock, "Book not found.\n", 17, 0);
                }
            } else if (strcmp(command, "EXIT") == 0) {
                send(client_sock, "Goodbye!\n", 9, 0);
                break;
            }
        }

        close(client_sock);
    }

    return 0;
}
