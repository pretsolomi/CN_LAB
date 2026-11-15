/*Write a Server Socket Program that binds to port number 8890 and listens for
connection from clients. The IP address of the Server is the localhost (127.0.0.1). */
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int server_fd, client_sock;
    struct sockaddr_in server;
    char buffer[1024] = {0};
    char *reply = "Hello from server";
    socklen_t len = sizeof(server);

    // 1. Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Configure localhost (127.0.0.1) and port 8890
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(8890);

    // 3. Bind and Listen
    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Bind failed\n");
        return 1;
    }
    listen(server_fd, 3);
    printf("Waiting for client...\n");

    // 4. Accept connection
    client_sock = accept(server_fd, (struct sockaddr *)&server, &len);

    // 5. Read from client and send reply
    read(client_sock, buffer, sizeof(buffer) - 1);
    printf("Client: %s\n", buffer);
    send(client_sock, reply, strlen(reply), 0);

    // 6. Close sockets
    close(client_sock);
    close(server_fd);
    return 0;
}
