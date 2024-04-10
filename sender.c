#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 8080
#define BUFSIZE 4096

int main() {
    int sockfd, numbytes;
    char buf[BUFSIZE];
    struct sockaddr_in server_addr;
    struct hostent *host;

    // Resolve localhost
    if ((host = gethostbyname("localhost")) == NULL) {
        perror("gethostbyname");
        exit(1);
    }

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // Initialize server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    memset(&(server_addr.sin_zero), '\0', 8);

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect");
        exit(1);
    }

    // Send HTTP GET request
    const char *request = "GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";
    if (send(sockfd, request, strlen(request), 0) == -1) {
        perror("send");
        exit(1);
    }

    // Receive response
    while ((numbytes = recv(sockfd, buf, BUFSIZE - 1, 0)) > 0) {
        buf[numbytes] = '\0';
        printf("%s", buf);
    }

    // Check for errors or EOF
    if (numbytes == -1) {
        perror("recv");
        exit(1);
    }

    // Close socket
    close(sockfd);

    return 0;
}
