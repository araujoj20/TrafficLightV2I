#include "../inc/udp.h"


int createSocket(){
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Enable Broadcast
    int broadcastEnable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) == -1){
        perror("setsockopt (SO_BROADCAST) failed");
        exit(EXIT_FAILURE);
    }

    return sockfd;
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
    //broadcast_addr->sin_addr.s_addr = htonl(INADDR_BROADCAST); // another type of broadcast
    broadcast_addr->sin_addr.s_addr = inet_addr(BROADCAST_IP);
}
