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
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE * 10];
    socklen_t addr_len = sizeof(client_addr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP Server listening on port %d...\n", PORT);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);

        if (strcmp(buffer, "stop") == 0) {
            printf("Client disconnected.\n");
            break;
        }

        printf("File request: %s\n", buffer);
        memset(response, 0, sizeof(response));
        analyze_file(buffer, response);

        sendto(sockfd, response, strlen(response), 0, (struct sockaddr *)&client_addr, addr_len);
    }

    close(sockfd);
    return 0;
}

