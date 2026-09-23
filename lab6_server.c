/*Implement the bit stuffing algorithm in C for both server and client.*/
/*Server*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 2048
#define FLAG "01111110"
#define FLAG_LEN 8

// Bit de-stuffing logic
void bit_destuff(const char *input, char *output) {
    int count = 0;
    int j = 0;

    for (int i = 0; input[i] != '\0'; i++) {
        output[j++] = input[i];

        if (input[i] == '1') {
            count++;
            // If five consecutive '1's are followed by '0', drop the '0'
            if (count == 5) {
                if (input[i + 1] == '0') {
                    i++; // Skip stuffed bit
                }
                count = 0;
            }
        } else {
            count = 0;
        }
    }
    output[j] = '\0';
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char stuffed_payload[BUFFER_SIZE] = {0};
    char destuffed_data[BUFFER_SIZE] = {0};

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse address and port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("[Server] Listening on port %d...\n", PORT);

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Read transmitted frame
    ssize_t bytes_read = read(new_socket, buffer, BUFFER_SIZE - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("[Server] Received Frame: %s\n", buffer);

        // Verify and strip the FLAG delimiters
        int total_len = strlen(buffer);
        if (total_len >= 2 * FLAG_LEN &&
            strncmp(buffer, FLAG, FLAG_LEN) == 0 &&
            strncmp(buffer + total_len - FLAG_LEN, FLAG, FLAG_LEN) == 0) {

            // Extract the middle stuffed payload
            int payload_len = total_len - (2 * FLAG_LEN);
            strncpy(stuffed_payload, buffer + FLAG_LEN, payload_len);
            stuffed_payload[payload_len] = '\0';

            // De-stuff the data
            bit_destuff(stuffed_payload, destuffed_data);

            printf("\n--- Extraction Summary ---\n");
            printf("Extracted Payload : %s\n", stuffed_payload);
            printf("De-stuffed Data   : %s\n", destuffed_data);
        } else {
            printf("[Server] Error: Frame delimiters invalid or missing.\n");
        }
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
