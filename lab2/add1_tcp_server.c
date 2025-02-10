#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void analyze_file(const char *filename, char *result) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        strcpy(result, "File not found.\n");
        return;
    }

    int alphabets = 0, digits = 0, spaces = 0, others = 0, lines = 0;
    long file_size = 0;
    char ch;

    while ((ch = fgetc(file)) != EOF) {
        file_size++;
        if (isalpha(ch))
            alphabets++;
        else if (isdigit(ch))
            digits++;
        else if (isspace(ch)) {
            spaces++;
            if (ch == '\n')
                lines++;
        } else {
            others++;
        }
    }

    rewind(file);
    char file_contents[BUFFER_SIZE];
    fread(file_contents, 1, file_size, file);
    file_contents[file_size] = '\0';

    sprintf(result, "File Size: %ld bytes\nAlphabets: %d\nDigits: %d\nSpaces: %d\nLines: %d\nOther Characters: %d\nContents:\n%s",
            file_size, alphabets, digits, spaces, lines, others, file_contents);

    fclose(file);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char response[BUFFER_SIZE * 10];

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
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

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            read(new_socket, buffer, BUFFER_SIZE);

            if (strcmp(buffer, "stop") == 0) {
                printf("Client disconnected.\n");
                break;
            }

            printf("File request: %s\n", buffer);
            memset(response, 0, sizeof(response));
            analyze_file(buffer, response);
            send(new_socket, response, strlen(response), 0);
        }
        close(new_socket);
    }

    return 0;
}

