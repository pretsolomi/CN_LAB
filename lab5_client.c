/*
Create a chat project which allows two program to send messages to each other. The
Messages must be taken from the users through the keyboard. 
*/
/*CLIENT*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char message[BUFFER_SIZE] = {0};

    // 1. Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("[b.c] Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("[b.c] Invalid address");
        close(sock);
        return -1;
    }

    // 2. Connect to server
    printf("[b.c] Connecting to server...\n");
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("[b.c] Connection failed");
        close(sock);
        return -1;
    }

    printf("[b.c] Connected to a.c! Start chatting (type 'exit' to quit).\n\n");

    // 3. Chat loop
    while (1) {
        memset(message, 0, BUFFER_SIZE);
        memset(buffer, 0, BUFFER_SIZE);

        // Get message from keyboard
        printf("You (b.c): ");
        if (fgets(message, BUFFER_SIZE, stdin) == NULL) break;

        send(sock, message, strlen(message), 0);

        if (strncmp(message, "exit", 4) == 0) {
            printf("[b.c] Chat ended.\n");
            break;
        }

        // Receive reply from a.c
        int valread = read(sock, buffer, BUFFER_SIZE - 1);
        if (valread <= 0) {
            printf("\n[b.c] Connection closed by a.c.\n");
            break;
        }
        buffer[valread] = '\0';
        printf("a.c says: %s", buffer);

        if (strncmp(buffer, "exit", 4) == 0) {
            printf("[b.c] Chat ended by remote user.\n");
            break;
        }
    }

    close(sock);
    return 0;
}
