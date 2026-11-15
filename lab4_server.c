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
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock;
    struct sockaddr_in server;
    char *msg = "Hello from client";
    char buffer[1024] = {0};

    // 1. Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Define server address
    server.sin_family = AF_INET;
    server.sin_port = htons(8890);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 3. Connect to server
    connect(sock, (struct sockaddr *)&server, sizeof(server));

    // 4. Send message & read reply
    send(sock, msg, strlen(msg), 0);
    read(sock, buffer, sizeof(buffer));
    printf("Server Reply: %s\n", buffer);

    // 5. Close socket
    close(sock);
    return 0;
}
