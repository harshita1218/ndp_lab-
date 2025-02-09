#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

#define PORT 8080
#define MAX_LEN 1024

void handle_registration_number(int client_sock) {
    char response[MAX_LEN];
    snprintf(response, MAX_LEN, "Name: Harshita\nAddress: New Delhi\nHandled by PID: %d\n", getpid());
    send(client_sock, response, strlen(response), 0);
}

void handle_student_name(int client_sock) {
    char response[MAX_LEN];
    snprintf(response, MAX_LEN, "Department: CSE\nSemester: 6\nSection: A\nCourses: OS, DS, Networks\nHandled by PID: %d\n", getpid());
    send(client_sock, response, strlen(response), 0);
}

void handle_subject_code(int client_sock) {
    char response[MAX_LEN];
    snprintf(response, MAX_LEN, "Marks in Subject: 85\nHandled by PID: %d\n", getpid());
    send(client_sock, response, strlen(response), 0);
}

int main() {
    int server_fd, client_sock;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
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
        client_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (client_sock < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        char buffer[MAX_LEN] = {0};
        read(client_sock, buffer, MAX_LEN);
        printf("Client Request: %s\n", buffer);

        int pid = fork();
        if (pid == 0) {  // Child process
            if (strncmp(buffer, "1", 1) == 0) {
                handle_registration_number(client_sock);
            } else if (strncmp(buffer, "2", 1) == 0) {
                handle_student_name(client_sock);
            } else if (strncmp(buffer, "3", 1) == 0) {
                handle_subject_code(client_sock);
            }
            close(client_sock);
            exit(0);
        } else {  // Parent process
            wait(NULL);
            close(client_sock);
        }
    }

    return 0;
}
