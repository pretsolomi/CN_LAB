#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

int main() {
    int sock;
    struct sockaddr_in server;
    struct hostent *he;
    char *host = "www.google.com";
    char req[100], reply[4096];
    int len;

    // 1. Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    // 2. Get IP from URL
    he = gethostbyname(host);
    struct in_addr **addr_list = (struct in_addr **)he->h_addr_list;
    printf("IP Address: %s\n", inet_ntoa(*addr_list[0]));

    // 3. Setup server address & connect
    server.sin_family = AF_INET;
    server.sin_port = htons(80);
    server.sin_addr = *addr_list[0]; // Directly copy the resolved IP

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connection failed\n");
        return 1;
    }

    // 4. Send HTTP GET request
    sprintf(req, "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", host);
    send(sock, req, strlen(req), 0);

    // 5. Receive and print response
    while ((len = recv(sock, reply, sizeof(reply) - 1, 0)) > 0) {
        reply[len] = '\0';
        printf("%s", reply);
    }

    close(sock);
    return 0;
}
