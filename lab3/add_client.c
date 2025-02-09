#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void add_subtract(int sockfd) {
    int a, b;
    printf("Enter two integers: ");
    scanf("%d %d", &a, &b);

    write(sockfd, &a, sizeof(int));
    write(sockfd, &b, sizeof(int));

    int sum, difference;
    read(sockfd, &sum, sizeof(int));
    read(sockfd, &difference, sizeof(int));

    printf("Sum: %d\n", sum);
    printf("Difference: %d\n", difference);
}

void solve_linear_equation(int sockfd) {
    int a, b;
    
    // Get user input for the coefficients
    printf("Enter coefficients a and b (for equation ax + b = 0): ");
    scanf("%d %d", &a, &b);

    // Send coefficients to the server
    write(sockfd, &a, sizeof(int));
    write(sockfd, &b, sizeof(int));

    // Receive the result from the server
    float x;
    read(sockfd, &x, sizeof(float));

    if (x == -9999.0) {
        printf("The equation has no solution (undefined for a = 0).\n");
    } else {
        printf("Solution for x: %.2f\n", x);
    }
}


void multiply_matrices(int sockfd) {
    int r1, c1, r2, c2;
    printf("Enter rows and columns of first matrix: ");
    scanf("%d %d", &r1, &c1);
    printf("Enter rows and columns of second matrix: ");
    scanf("%d %d", &r2, &c2);

    write(sockfd, &r1, sizeof(int));
    write(sockfd, &c1, sizeof(int));
    write(sockfd, &r2, sizeof(int));
    write(sockfd, &c2, sizeof(int));

    if (c1 != r2) {
        printf("Matrix multiplication not possible.\n");
        return;
    }

    int matrix1[r1][c1], matrix2[r2][c2];
    printf("Enter elements of first matrix:\n");
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c1; j++) {
            scanf("%d", &matrix1[i][j]);
        }
    }

    printf("Enter elements of second matrix:\n");
    for (int i = 0; i < r2; i++) {
        for (int j = 0; j < c2; j++) {
            scanf("%d", &matrix2[i][j]);
        }
    }

    write(sockfd, matrix1, sizeof(matrix1));
    write(sockfd, matrix2, sizeof(matrix2));

    int status;
    read(sockfd, &status, sizeof(int));
    if (status == -1) {
        printf("Matrix multiplication failed.\n");
        return;
    }

    int result[r1][c2];
    read(sockfd, result, sizeof(result));
    printf("Result of matrix multiplication:\n");
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c2; j++) {
            printf("%d ", result[i][j]);
        }
        printf("\n");
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    int option;
    while (1) {
        printf("\nMenu:\n");
        printf("1. Add/Subtract two integers\n");
        printf("2. Solve linear equation ax + b = 0\n");
        printf("3. Multiply two matrices\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &option);

        write(sockfd, &option, sizeof(int));

        switch (option) {
            case 1:
                add_subtract(sockfd);
                break;
            case 2:
                solve_linear_equation(sockfd);
                break;
            case 3:
                multiply_matrices(sockfd);
                break;
            case 4:
                close(sockfd);
                printf("Client exiting...\n");
                exit(0);
            default:
                printf("Invalid option. Try again.\n");
                break;
        }
    }

    return 0;
}

