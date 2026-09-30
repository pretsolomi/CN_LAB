/*
    Create a WLAN network between multiple computers. Use one of the Computers to host the WLAN and connect to this Computer using wi-fi from another computer. 
    Test the connection using ‘ping’. Write a report mentioning the steps involved.
*/
/*wlan client*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8890 //8080 or 8890 test both

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char *message = "Hello from Computer 2 over the wireless LAN!";

    // 1. Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nSocket creation error\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 2. Convert and set Computer 1's WLAN IP address
    if (inet_pton(AF_INET, "10.42.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported\n");
        return -1;
    }

    // 3. Connect to the WLAN Server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed. Ensure Wi-Fi is connected and IP is correct.\n");
        return -1;
    }

    // 4. Send message over Wi-Fi
    send(sock, message, strlen(message), 0);
    printf("Message successfully sent over WLAN to server.\n");

    // Cleanup
    close(sock);
    return 0;
}
