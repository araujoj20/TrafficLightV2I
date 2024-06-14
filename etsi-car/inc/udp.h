#ifndef UDP_H
#define UDP_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 12345
#define STATION_UNIQUE_ID 0x12345454
#define BROADCAST_IP "192.168.1.255"
#define MAX_MSG_SIZE 2048

void createSocket(int* sockfd);
void sendMessage(int sockfd, struct sockaddr_in *broadcast_addr, char *buffer, unsigned int bytes_enc);
void configureBroadcast(struct sockaddr_in *broadcast_addr);
void rxBroadcast(int* sockfd, struct sockaddr_in *broadcast_addr, socklen_t *addr_len);
int receiveMessage(int sockfd, char *buffer, struct sockaddr *client_addr, socklen_t *addr_len);

#endif // UDP_H