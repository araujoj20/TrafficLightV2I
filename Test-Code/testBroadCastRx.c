#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345
#define MAX_MSG_SIZE 1024

void createSocket(int *sockfd) {
    if ((*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    int broadcastEnable = 1;
    if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &broadcastEnable, sizeof(broadcastEnable)) == -1) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }
}

void configureServerAddress(struct sockaddr_in *server_addr) {
    memset(server_addr, 0, sizeof(*server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(PORT);
    server_addr->sin_addr.s_addr = htonl(INADDR_ANY);
}

void receiveMessage(int sockfd, char *buffer, struct sockaddr_in *client_addr) {
    socklen_t addr_len = sizeof(*client_addr);
    int bytes_recv = recvfrom(sockfd, buffer, MAX_MSG_SIZE, 0, (struct sockaddr *)client_addr, &addr_len);
    if (bytes_recv == -1) {
        perror("Receive failed");
        return;
    }
    buffer[bytes_recv] = '\0';
}

int main() {
    int sockfd;
    createSocket(&sockfd);
    struct sockaddr_in server_addr, client_addr;
    configureServerAddress(&server_addr);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    char in_buffer[MAX_MSG_SIZE];
    while(1) {
        receiveMessage(sockfd, in_buffer, &client_addr);
        printf("Received message: %s\n", in_buffer);
    }

    close(sockfd);
    return 0;
}
