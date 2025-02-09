#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to sort a string in ascending order
void sort_ascending(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len - 1; i++) {
        for (int j = i + 1; j < len; j++) {
            if (str[i] > str[j]) {
                char temp = str[i];
                str[i] = str[j];
                str[j] = temp;
            }
        }
    }
}

// Function to sort a string in descending order
void sort_descending(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len - 1; i++) {
        for (int j = i + 1; j < len; j++) {
            if (str[i] < str[j]) {
                char temp = str[i];
                str[i] = str[j];
                str[j] = temp;
            }
        }
    }
}

int main() {
    int sockfd, new_sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t client_len = sizeof(client_addr);

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Allow socket reuse
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(sockfd, 5) < 0) {
        perror("Listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is running and waiting for connections...\n");

    // Accept connection from client
    if ((new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len)) < 0) {
        perror("Accept failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Read input from client
    read(new_sockfd, buffer, BUFFER_SIZE);
    printf("Received string from client: %s\n", buffer);

    // Create a child process
    pid_t pid = fork();

    if (pid == 0) {  // Child process
        char numbers[BUFFER_SIZE] = {0};
        int num_index = 0;

        // Extract numbers from the input string
        for (int i = 0; buffer[i] != '\0'; i++) {
            if (isdigit(buffer[i])) {
                numbers[num_index++] = buffer[i];
            }
        }
        numbers[num_index] = '\0';

        // Sort the numbers in ascending order
        sort_ascending(numbers);

        // Prepare the result with PID
        char result[BUFFER_SIZE];
        snprintf(result, BUFFER_SIZE, "Child process (PID: %d), sorted numbers: %s", getpid(), numbers);

        // Send the result back to the client
        write(new_sockfd, result, strlen(result));
    } else {  // Parent process
        char letters[BUFFER_SIZE] = {0};
        int letter_index = 0;

        // Extract letters from the input string
        for (int i = 0; buffer[i] != '\0'; i++) {
            if (isalpha(buffer[i])) {
                letters[letter_index++] = buffer[i];
            }
        }
        letters[letter_index] = '\0';

        // Sort the letters in descending order
        sort_descending(letters);

        // Prepare the result with PID
        char result[BUFFER_SIZE];
        snprintf(result, BUFFER_SIZE, "Parent process (PID: %d), sorted letters: %s", getpid(), letters);

        // Send the result back to the client
        write(new_sockfd, result, strlen(result));
    }

    close(new_sockfd);
    close(sockfd);
    return 0;
}

