/*Create a LAN network between two computers. Test the connection using ‘ping’. Write
a report mentioning the steps involved.
*/
//CLIENT  testing code

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8890 //8080 notworking

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from Computer 2 over the LAN!";

    // 1. Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 2. Convert IP address to binary
    if (inet_pton(AF_INET, "192.168.1.10", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // 3. Connect to Server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    // 4. Send message
    send(sock, hello, strlen(hello), 0);
    printf("Hello message sent from client to server.\n");

    // Cleanup
    close(sock);
    return 0;
}
