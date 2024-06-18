#include "../inc/udp.h"

/**
 * @brief Creates a socket for UDP communication and enables broadcast.
 * 
 * @param sockfd Pointer to an integer where the socket file descriptor will be stored.
 */
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

/**
 * @brief Sends a message via the specified socket.
 * 
 * @param sockfd The socket file descriptor to use for sending the message.
 * @param broadcast_addr Pointer to a struct sockaddr_in containing the broadcast address information.
 * @param buffer Pointer to the buffer containing the message to be sent.
 * @param bytes_enc The number of bytes in the buffer to send.
 */
void sendMessage(int sockfd, struct sockaddr_in *broadcast_addr, char *buffer, unsigned int bytes_enc){
    if (sendto(sockfd, buffer, bytes_enc, 0, (struct sockaddr *)broadcast_addr, sizeof(*broadcast_addr)) == -1) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Configures the sockaddr_in structure for broadcast.
 * 
 * @param broadcast_addr Pointer to a struct sockaddr_in to be configured.
 */
void configureBroadcast(struct sockaddr_in *broadcast_addr){
    memset(broadcast_addr, 0, sizeof(*broadcast_addr));
    broadcast_addr->sin_family = AF_INET;
    broadcast_addr->sin_port = htons(PORT);
    //broadcast_addr->sin_addr.s_addr = htonl(INADDR_BROADCAST); // another type of broadcast
    broadcast_addr->sin_addr.s_addr = inet_addr(BROADCAST_IP);
}

/**
 * @brief Binds the socket to the specified broadcast address for receiving messages.
 * 
 * @param sockfd Pointer to the socket file descriptor.
 * @param broadcast_addr Pointer to a struct sockaddr_in containing the broadcast address information.
 * @param addr_len Pointer to a socklen_t containing the size of the broadcast address structure.
 */
void rxBroadcast(int* sockfd, struct sockaddr_in *broadcast_addr, socklen_t *addr_len){

    if (bind(*sockfd, (struct sockaddr *)broadcast_addr, *addr_len) == -1){
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Receives a message via the specified socket.
 * 
 * @param sockfd The socket file descriptor to use for receiving the message.
 * @param buffer Pointer to the buffer where the received message will be stored.
 * @param client_addr Pointer to a struct sockaddr to store the address of the sender.
 * @param addr_len Pointer to a socklen_t containing the size of the client address structure.
 * @return The number of bytes received, or -1 if the receive operation failed.
 */
int receiveMessage(int sockfd, char *buffer, struct sockaddr *client_addr, socklen_t *addr_len) {
    int bytes_received = recvfrom(sockfd, buffer, MAX_MSG_SIZE, 0, client_addr, addr_len);
    if (bytes_received == -1) {
        perror("Receive failed");
        return -1;
    }
    return bytes_received;
}
