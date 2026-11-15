/*
Create a chat project which allows two program to send messages to each other. The
Messages must be taken from the users through the keyboard. 
*/
/*CLIENT*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int sock;
    struct sockaddr_in addr = {AF_INET, htons(8080)};
    char buf[1024];

    // 1. Create and Connect
    sock = socket(AF_INET, SOCK_STREAM, 0);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    // 2. Chat Loop
    while (1) {
        printf("Client: ");
        fgets(buf, 1024, stdin);
        send(sock, buf, strlen(buf), 0);
        if (strncmp(buf, "exit", 4) == 0) break;

        int n = read(sock, buf, 1023);
        buf[n] = '\0';
        printf("Server: %s", buf);
        if (strncmp(buf, "exit", 4) == 0) break;
    }

    close(sock);
    return 0;
}
