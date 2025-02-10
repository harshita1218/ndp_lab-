#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 12345
#define MAX_CLIENTS 5

int client_count = 0;
pthread_mutex_t lock;

void *handle_client(void *socket_desc) {
    int client_sock = *(int *)socket_desc;
    free(socket_desc);
    char client_message[1024];
    memset(client_message, 0, sizeof(client_message));

    // Receive message from client
    if (recv(client_sock, client_message, sizeof(client_message), 0) > 0) {
        printf("Received: %s\n", client_message);

        // Append message to the file
        pthread_mutex_lock(&lock);
        FILE *file = fopen("textfile.txt", "a");
        if (file == NULL) {
            perror("Error opening file");
            pthread_mutex_unlock(&lock);
            close(client_sock);
            return NULL;
        }
        fprintf(file, "%s ", client_message);
        fclose(file);
        pthread_mutex_unlock(&lock);
    }

    pthread_mutex_lock(&lock);
    client_count++;
    if (client_count > 2) {
        // Send termination message to the client
        char *terminate_message = "terminate session";
        send(client_sock, terminate_message, strlen(terminate_message), 0);
        printf("Terminating session: too many clients connected.\n");
        pthread_mutex_unlock(&lock);
        close(client_sock);
        exit(0);  // Exit the server
    }
    pthread_mutex_unlock(&lock);

    close(client_sock);
    return NULL;
}

int main() {
    int server_sock, client_sock, *new_sock;
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);

    // Initialize mutex
    pthread_mutex_init(&lock, NULL);

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Could not create socket");
        return 1;
    }
    printf("Server socket created.\n");

    // Set SO_REUSEADDR to allow reusing the port
    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_sock);
        return 1;
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind
    if (bind(server_sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        close(server_sock);
        return 1;
    }
    printf("Bind done.\n");

    // Listen for incoming connections
    listen(server_sock, MAX_CLIENTS);
    printf("Waiting for incoming connections...\n");

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client, &client_len);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }
        printf("Connection accepted.\n");

        pthread_t client_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;

        // Create a new thread for each client
        if (pthread_create(&client_thread, NULL, handle_client, (void *)new_sock) < 0) {
            perror("Could not create thread");
            free(new_sock);
            continue;
        }
        pthread_detach(client_thread);
    }

    close(server_sock);
    pthread_mutex_destroy(&lock);
    return 0;
}

