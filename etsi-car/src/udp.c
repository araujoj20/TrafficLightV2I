#include "../inc/udp.h"


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

void configureBroadcast(struct sockaddr_in *broadcast_addr){
    memset(broadcast_addr, 0, sizeof(*broadcast_addr));
    broadcast_addr->sin_family = AF_INET;
    broadcast_addr->sin_port = htons(PORT);
    //broadcast_addr->sin_addr.s_addr = htonl(INADDR_BROADCAST); // another type of broadcast
    broadcast_addr->sin_addr.s_addr = inet_addr(BROADCAST_IP);

}

void rxBroadcast(int* sockfd, struct sockaddr_in *broadcast_addr, socklen_t *addr_len) {
    
    if (bind(*sockfd, (struct sockaddr *)broadcast_addr, *addr_len) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
}


void sendMessage(int sockfd, struct sockaddr_in *broadcast_addr, char *buffer, unsigned int bytes_enc){
    if (sendto(sockfd, buffer, bytes_enc, 0, (struct sockaddr *)broadcast_addr, sizeof(*broadcast_addr)) == -1) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }
}

int receiveMessage(int sockfd, char *buffer, struct sockaddr *client_addr, socklen_t *addr_len){
    int bytes_received = recvfrom(sockfd, buffer, MAX_MSG_SIZE, 0, client_addr, addr_len);
    if (bytes_received == -1) {
        perror("Receive failed");
        return -1;
    }
    return bytes_received;
}