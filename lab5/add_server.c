#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 12345
#define MAX_SEATS 50
#define BUFFER_SIZE 1024

typedef struct {
    int route1_seats;  // Seats available for Route 1 (Source A to Destination B)
    int route2_seats;  // Seats available for Route 2 (Source C to Destination D)
} SeatDatabase;

SeatDatabase db = {MAX_SEATS, MAX_SEATS};  // Initializing both routes with MAX_SEATS
pthread_mutex_t lock;  // Mutex for thread-safe access to the database

void *handle_client(void *socket_desc) {
    int client_sock = *(int *)socket_desc;
    free(socket_desc);

    char client_message[BUFFER_SIZE];
    memset(client_message, 0, sizeof(client_message));

    // Receive the client's booking request
    if (recv(client_sock, client_message, sizeof(client_message), 0) > 0) {
        int route, seats_requested;
        sscanf(client_message, "%d %d", &route, &seats_requested);

        pthread_mutex_lock(&lock);

        int *available_seats = (route == 1) ? &db.route1_seats : &db.route2_seats;
        char response[BUFFER_SIZE];

        if (seats_requested <= *available_seats) {
            *available_seats -= seats_requested;
            snprintf(response, sizeof(response), "Booking successful! Seats remaining for Route %d: %d", route, *available_seats);
            printf("Route %d: %d seats booked, %d seats remaining.\n", route, seats_requested, *available_seats);
        } else {
            snprintf(response, sizeof(response), "Booking failed! Not enough seats available for Route %d. Seats remaining: %d", route, *available_seats);
            printf("Booking failed for Route %d. Requested: %d, Available: %d\n", route, seats_requested, *available_seats);
        }

        pthread_mutex_unlock(&lock);

        // Send the response to the client
        send(client_sock, response, strlen(response), 0);
    }

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
    listen(server_sock, 5);
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

