/*  Write a Socket Program to connect to a remote server (e.g. www.google.com) and
send HTTP request to the server and display the reply from the remote server. The
IP Address must be obtained from the URL. 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define PORT 80
#define BUFFER_SIZE 4096

int main(int argc, char *argv[])
{
    int socket_desc;
    struct sockaddr_in server;
    struct hostent *he;
    char *hostname = "www.google.com";
    char message[1000], server_reply[BUFFER_SIZE];
    int recv_size;

    // 1. Create TCP socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        perror("Could not create socket");
        return 1;
    }
    printf("-> Socket created successfully\n");

    // 2. Resolve hostname to IP address
    if ((he = gethostbyname(hostname)) == NULL) {
        herror("gethostbyname failed");
        return 1;
    }

    // Cast the h_addr_in to struct in_addr
    struct in_addr **addr_list = (struct in_addr **)he->h_addr_list;
    char ip[100];
    strcpy(ip, inet_ntoa(*addr_list[0]));
    printf("-> Resolved %s to IP: %s\n", hostname, ip);

    // 3. Setup server structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons(PORT);

    // 4. Connect to remote server
    if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connect failed. Error");
        return 1;
    }
    printf("-> Connected to %s on port %d\n\n", hostname, PORT);

    // 5. Send HTTP GET Request
    sprintf(message, "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", hostname);
    if (send(socket_desc, message, strlen(message), 0) < 0) {
        perror("Send failed");
        return 1;
    }
    printf("-> HTTP Request sent:\n%s\n", message);

    // 6. Receive and display the reply from the remote server
    printf("-> Server Response:\n");
    printf("----------------------------------------\n");
    while ((recv_size = recv(socket_desc, server_reply, BUFFER_SIZE - 1, 0)) > 0) {
        server_reply[recv_size] = '\0';
        printf("%s", server_reply);
    }

    if (recv_size < 0) {
        perror("Receive failed");
    }

    printf("\n----------------------------------------\n");

    // 7. Close the socket
    close(socket_desc);
    return 0;
}
