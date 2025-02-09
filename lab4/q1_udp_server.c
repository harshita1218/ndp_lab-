#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

#define PORT 8080
#define MAX_LEN 1024

void handle_registration_number(int server_sock, struct sockaddr_in *client_addr, socklen_t addr_len) {
    char response[MAX_LEN];
    snprintf(response, MAX_LEN, "Name: Harshita\nAddress: New Delhi\nHandled by PID: %d\n", getpid());
    sendto(server_sock, response, strlen(response), 0, (struct sockaddr *)client_addr, addr_len);
}

void handle_student_name(int server_sock, struct sockaddr_in *client_addr, socklen_t addr_len) {
    char response[MAX_LEN];
    snprintf(response, MAX_LEN, "Department: CSE\nSemester: 6\nSection: A\nCourses: OS, DS, Networks\nHandled by PID: %d\n", getpid());
    sendto(server_sock, response, strlen(response), 0, (struct sockaddr *)client_addr, addr_len);
}

void handle_subject_code(int server_sock, struct sockaddr_in *client_addr, socklen_t addr_len) {
    char response[MAX_LEN];
    snprintf(response, MAX_LEN, "Marks in Subject: 85\nHandled by PID: %d\n", getpid());
    sendto(server_sock, response, strlen(response), 0, (struct sockaddr *)client_addr, addr_len);
}

int main() {
    int server_sock;
    struct sockaddr_in server_addr, client_addr;
    char buffer[MAX_LEN];
    socklen_t addr_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    printf("UDP Server listening on port %d...\n", PORT);

    while (1) {
        memset(buffer, 0, MAX_LEN);
        recvfrom(server_sock, buffer, MAX_LEN, 0, (struct sockaddr *)&client_addr, &addr_len);
        printf("Client Request: %s\n", buffer);

        int pid = fork();
        if (pid == 0) {  // Child process
            if (strncmp(buffer, "1", 1) == 0) {
                handle_registration_number(server_sock, &client_addr, addr_len);
            } else if (strncmp(buffer, "2", 1) == 0) {
                handle_student_name(server_sock, &client_addr, addr_len);
            } else if (strncmp(buffer, "3", 1) == 0) {
                handle_subject_code(server_sock, &client_addr, addr_len);
            }
            exit(0);
        }
    }

    return 0;
}

