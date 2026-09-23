/* Write a Client Socket Program that connects to the server program which you have
written in program 3. Retrieve the Reply from the Server and display the output.
*/
/*
   Client Socket Program

   This program:
   1. Creates a TCP socket.
   2. Connects to the server at 127.0.0.1:8890.
   3. Sends a message to the server.
   4. Receives the reply from the server.
   5. Displays the reply.
*/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8890

int main() {
    int client_fd;
    ssize_t valread;
    struct sockaddr_in serv_addr;
    const char *hello = "Hello from client";
    char buffer[1024] = {0};

    printf("Client running...\n");

    // 1. Create TCP socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // 2. Set server address parameters
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 3. Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        close(client_fd);
        return -1;
    }

    // 4. Connect to server
    if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        close(client_fd);
        return -1;
    }

    printf("Connected to server.\n");

    // 5. Send message
    send(client_fd, hello, strlen(hello), 0);
    printf("Sent: %s\n", hello);

    // 6. Receive reply
    valread = read(client_fd, buffer, sizeof(buffer) - 1);
    if (valread > 0) {
        buffer[valread] = '\0';
        printf("Server: %s\n", buffer);
    } else {
        perror("Read failed or connection closed");
    }

    // 7. Close socket
    close(client_fd);
    return 0;
}
