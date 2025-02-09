#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_add_subtract(int client_sock) {
    int a, b;
    read(client_sock, &a, sizeof(int));
    read(client_sock, &b, sizeof(int));

    int sum = a + b;
    int difference = a - b;

    write(client_sock, &sum, sizeof(int));
    write(client_sock, &difference, sizeof(int));
}

void handle_linear_equation(int client_sock) {
    int a, b;
    
    // Read the coefficients a and b from the client
    read(client_sock, &a, sizeof(int));
    read(client_sock, &b, sizeof(int));

    if (a == 0) {
        float undefined = -9999.0;  // Represent undefined or no solution
        write(client_sock, &undefined, sizeof(float));
    } else {
        float x = -(float)b / a;
        write(client_sock, &x, sizeof(float));  // Send the solution back
    }
}


void handle_matrix_multiplication(int client_sock) {
    int r1, c1, r2, c2;
    read(client_sock, &r1, sizeof(int));
    read(client_sock, &c1, sizeof(int));
    read(client_sock, &r2, sizeof(int));
    read(client_sock, &c2, sizeof(int));

    if (c1 != r2) {
        int error = -1;
        write(client_sock, &error, sizeof(int));
        return;
    }

    int matrix1[r1][c1], matrix2[r2][c2], result[r1][c2];
    memset(result, 0, sizeof(result));

    // Read matrix1 and matrix2
    read(client_sock, matrix1, sizeof(matrix1));
    read(client_sock, matrix2, sizeof(matrix2));

    // Perform matrix multiplication
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c2; j++) {
            for (int k = 0; k < c1; k++) {
                result[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }

    // Send success flag and the result
    int success = 0;
    write(client_sock, &success, sizeof(int));
    write(client_sock, result, sizeof(result));
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create socket
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_sock, 5) < 0) {
        perror("Listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server is running...\n");

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        int option;
        read(client_sock, &option, sizeof(int));

        switch (option) {
            case 1:
                handle_add_subtract(client_sock);
                break;
            case 2:
                handle_linear_equation(client_sock);
                break;
            case 3:
                handle_matrix_multiplication(client_sock);
                break;
            case 4:
                close(client_sock);
                close(server_sock);
                printf("Server exiting...\n");
                exit(0);
            default:
                printf("Invalid option received.\n");
                break;
        }

        close(client_sock);
    }

    return 0;
}

