/*Implement the bit stuffing algorithm in C for both server and client.*/
/*client*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define FLAG "01111110"

// Bit stuffing logic
void bit_stuff(const char *input, char *output) {
    int count = 0;
    int j = 0;

    for (int i = 0; input[i] != '\0'; i++) {
        output[j++] = input[i];

        if (input[i] == '1') {
            count++;
            // Insert a '0' after five consecutive '1's
            if (count == 5) {
                output[j++] = '0';
                count = 0; // Reset counter after stuffing
            }
        } else {
            count = 0;
        }
    }
    output[j] = '\0';
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char raw_data[BUFFER_SIZE];
    char stuffed_data[BUFFER_SIZE * 2];
    char frame[BUFFER_SIZE * 2 + 32];

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Connect to localhost
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("[Client] Connected to server on port %d\n", PORT);
    printf("[Client] Enter binary data (e.g., 01111110111110): ");
    if (scanf("%s", raw_data) != 1) {
        close(sock);
        return 1;
    }

    // Perform bit stuffing
    bit_stuff(raw_data, stuffed_data);

    // Encapsulate into a frame: FLAG + Stuffed Data + FLAG
    snprintf(frame, sizeof(frame), "%s%s%s", FLAG, stuffed_data, FLAG);

    printf("\n--- Transmission Summary ---\n");
    printf("Original Data : %s\n", raw_data);
    printf("Stuffed Data  : %s\n", stuffed_data);
    printf("Framed Packet : %s\n", frame);

    // Send framed packet to server
    send(sock, frame, strlen(frame), 0);
    printf("[Client] Frame successfully sent to server.\n");

    close(sock);
    return 0;
}
