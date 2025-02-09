#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to calculate factorial of a number
int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

// Function to swap two characters
void swap(char *x, char *y) {
    char temp = *x;
    *x = *y;
    *y = temp;
}

// Function to generate all permutations of a string and store them in a dynamically allocated array
void permute(char *str, int left, int right, char **permutations, int *count) {
    if (left == right) {
        strcpy(permutations[*count], str);
        (*count)++;
    } else {
        for (int i = left; i <= right; i++) {
            swap((str + left), (str + i));
            permute(str, left + 1, right, permutations, count);
            swap((str + left), (str + i));  // backtrack
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t len = sizeof(client_addr);

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // Fill server information
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Server is running and waiting for client messages...\n");

    // Receive string from client
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, MSG_WAITALL, (struct sockaddr *)&client_addr, &len);
    buffer[n] = '\0';
    printf("Received string from client: %s\n", buffer);

    int num_permutations = factorial(strlen(buffer));
    char **permutations = (char **)malloc(num_permutations * sizeof(char *));
    for (int i = 0; i < num_permutations; i++) {
        permutations[i] = (char *)malloc(BUFFER_SIZE);
    }

    // Generate permutations
    int permutations_count = 0;
    permute(buffer, 0, strlen(buffer) - 1, permutations, &permutations_count);
    printf("Generated %d permutations.\n", permutations_count);

    // Send each permutation back to the client
    for (int i = 0; i < permutations_count; i++) {
        sendto(sockfd, permutations[i], strlen(permutations[i]), MSG_CONFIRM, (const struct sockaddr *)&client_addr, len);
    }

    printf("All permutations sent to client.\n");

    // Free allocated memory
    for (int i = 0; i < num_permutations; i++) {
        free(permutations[i]);
    }
    free(permutations);

    close(sockfd);
    return 0;
}

