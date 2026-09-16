/* Write a Socket Program to connect to a remote server (e.g. www.google.com) and
send HTTP request to the server and display the reply from the remote server. The
IP Address must be obtained from the URL. 
*/
#include <stdio.h>
#include <sys/socket.h>

int main(int argc, char *argv[])
{
    int socket_desc;

    // Create TCP socket (IPv4, TCP stream)
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc == -1)
    {
        printf("Could not create socket\n");
        return 1;
    }

    printf("Socket created successfully\n");

    return 0;
}
