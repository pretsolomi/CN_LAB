/*
    Create a WLAN network between multiple computers. Use one of the Computers to host the WLAN and connect to this Computer using wi-fi from another computer. 
    Test the connection using ‘ping’. Write a report mentioning the steps involved.
*/
/*wlan server*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8890  //8080 ot 8890 test both
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // 1. Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Bind socket to port 8080
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Listens on all interfaces (including wlan0)
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // 3. Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("WLAN Server is listening on port %d...\n", PORT);

    // 4. Accept client connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    // 5. Read data from wireless client
    read(new_socket, buffer, BUFFER_SIZE);
    printf("Received message over WLAN: %s\n", buffer);

    // Cleanup
    close(new_socket);
    close(server_fd);
    return 0;
}
