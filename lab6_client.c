/*Implement the bit stuffing algorithm in C for both server and client.*/
/*client*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define FLAG "01111110"

void bit_stuff(char *in, char *out) {
    int count = 0, j = 0;
    for (int i = 0; in[i] != '\0'; i++) {
        out[j++] = in[i];
        if (in[i] == '1') {
            count++;
            if (count == 5) {       // Insert '0' after five consecutive '1's
                out[j++] = '0';
                count = 0;
            }
        } else {
            count = 0;
        }
    }
    out[j] = '\0';
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {AF_INET, htons(PORT), inet_addr("127.0.0.1")};

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    char raw[100], stuffed[200], frame[250];
    printf("Enter binary data: ");
    scanf("%s", raw);

    bit_stuff(raw, stuffed);
    sprintf(frame, "%s%s%s", FLAG, stuffed, FLAG); // Add start & end flags

    printf("Stuffed Data : %s\n", stuffed);
    printf("Sent Frame   : %s\n", frame);

    send(sock, frame, strlen(frame), 0);
    close(sock);
    return 0;
}
