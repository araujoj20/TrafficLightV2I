#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 1234
#define MAX_MSG_SIZE 1024

void createSocket(int *sockfd);
void configureListenSocket(struct sockaddr_in *listen_addr);
void receiveMessage(int sockfd, struct sockaddr_in *listen_addr);

int main() {
    int sockfd;
    struct sockaddr_in listen_addr;

    createSocket(&sockfd);
    configureListenSocket(&listen_addr);

    receiveMessage(sockfd, &listen_addr);

    close(sockfd);

    return 0;
}

void createSocket(int *sockfd){
    // Create socket
    if ((*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to any IP address and specific port
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    if (bind(*sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
}

void configureListenSocket(struct sockaddr_in *listen_addr) {
    memset(listen_addr, 0, sizeof(*listen_addr));
    listen_addr->sin_family = AF_INET;
    listen_addr->sin_addr.s_addr = htonl(INADDR_ANY);
    listen_addr->sin_port = htons(PORT);
}

void receiveMessage(int sockfd, struct sockaddr_in *listen_addr) {
    char buffer[MAX_MSG_SIZE];
    int recv_len;
    socklen_t addr_len = sizeof(*listen_addr);
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < 5; i++) {
        if ((recv_len = recvfrom(sockfd, buffer, MAX_MSG_SIZE, 0, (struct sockaddr *)listen_addr, &addr_len)) == -1) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        }

        buffer[recv_len] = '\0';
        printf("Received message: %s\n", buffer);
    }
    printf("------------------------------------------------------------\n");
}
