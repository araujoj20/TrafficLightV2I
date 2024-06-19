#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 1234
#define BROADCAST_IP "192.168.1.255"

void createSocket(int *sockfd);
void sendMessage(int sockfd, struct sockaddr_in *broadcast_addr, char *buffer, unsigned int bytes_enc);
void configureBroadcast(struct sockaddr_in *broadcast_addr);

int main() {
    int sockfd;
    struct sockaddr_in broadcast_addr;
    char buffer[] = "TESTE UDP BROADCAST";
    unsigned int bytes_enc = strlen(buffer);

    createSocket(&sockfd);
    configureBroadcast(&broadcast_addr);
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < 5; i++){
        sendMessage(sockfd, &broadcast_addr, buffer, bytes_enc);
        printf("(IP: %s) Message Sent: %s \n", BROADCAST_IP, buffer);
    }
    close(sockfd);
    printf("------------------------------------------------------------\n");
    return 0;
}

void createSocket(int *sockfd){
    // Create socket
    if ((*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Enable Broadcast
    int broadcastEnable = 1;
    if (setsockopt(*sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) == -1){
        perror("setsockopt (SO_BROADCAST) failed");
        exit(EXIT_FAILURE);
    }
}

void sendMessage(int sockfd, struct sockaddr_in *broadcast_addr, char *buffer, unsigned int bytes_enc){
    if (sendto(sockfd, buffer, bytes_enc, 0, (struct sockaddr *)broadcast_addr, sizeof(*broadcast_addr)) == -1) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }
}

void configureBroadcast(struct sockaddr_in *broadcast_addr){
    memset(broadcast_addr, 0, sizeof(*broadcast_addr));
    broadcast_addr->sin_family = AF_INET;
    broadcast_addr->sin_port = htons(PORT);
    broadcast_addr->sin_addr.s_addr = inet_addr(BROADCAST_IP);
}
