/*Implement the bit stuffing algorithm in C for both server and client.*/
/*Server*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define FLAG "01111110"

void bit_destuff(char *in, char *out) {
    int count = 0, j = 0;
    for (int i = 0; in[i] != '\0'; i++) {
        out[j++] = in[i];
        if (in[i] == '1') {
            count++;
            if (count == 5) {
                if (in[i + 1] == '0') i++; // Skip the stuffed '0'
                count = 0;
            }
        } else {
            count = 0;
        }
    }
    out[j] = '\0';
}

int main() {
    int s_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {AF_INET, htons(PORT), INADDR_ANY};

    bind(s_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(s_fd, 3);
    printf("Server listening on port %d...\n", PORT);

    int new_sock = accept(s_fd, NULL, NULL);

    char frame[250] = {0}, payload[200] = {0}, original[100] = {0};
    read(new_sock, frame, sizeof(frame));
    printf("Received Frame  : %s\n", frame);

    // Strip the 8-bit FLAG from start and end
    int payload_len = strlen(frame) - 16;
    strncpy(payload, frame + 8, payload_len);
    payload[payload_len] = '\0';

    bit_destuff(payload, original);
    printf("De-stuffed Data : %s\n", original);

    close(new_sock);
    close(s_fd);
    return 0;
}
