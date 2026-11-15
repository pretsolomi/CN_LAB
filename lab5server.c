/*  Create a chat project which allows two program to send messages to each other. 
    The Messages must be taken from the users through the keyboard. 
*/
/*server*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int s_fd, new_sock;
    struct sockaddr_in addr = {AF_INET, htons(8080), INADDR_ANY};
    socklen_t len = sizeof(addr);
    char buf[1024];

    // 1. Create, Bind, Listen, Accept
    s_fd = socket(AF_INET, SOCK_STREAM, 0);
    bind(s_fd, (struct sockaddr *)&addr, len);
    listen(s_fd, 3);
    printf("Waiting for client...\n");
    new_sock = accept(s_fd, (struct sockaddr *)&addr, &len);

    // 2. Chat Loop
    while (1) {
        int n = read(new_sock, buf, 1023);
        buf[n] = '\0';
        printf("Client: %s", buf);
        if (strncmp(buf, "exit", 4) == 0) break;

        printf("Server: ");
        fgets(buf, 1024, stdin);
        send(new_sock, buf, strlen(buf), 0);
        if (strncmp(buf, "exit", 4) == 0) break;
    }

    close(new_sock);
    close(s_fd);
    return 0;
}
