#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "asn1-header/SPATEM.h"
#include "asn1-header/per_decoder.h"

#define PORT 12345
#define MAX_MSG_SIZE 1024


//uint8_t in_buffer[1024] = {2 , 2 , 18 , 52 , 86 , 120 , 64 , 135 , 52 , 2 , 1 , 228 , 238 , 233 , 151 , 147 , 22 , 77 , 0 , 0 , 32 , 66 , 64 , 0 , 0 , 4 , 0 , 0};

int main(int ac, char **av)
{   
    int sockfd;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    uint8_t buffer[MAX_MSG_SIZE];

    SPATEM_t spatem_example;
    asn_dec_rval_t dc;
    uint16_t index = 0;

    SPATEM_t* decoded_spatem = &spatem_example;

    memset(&spatem_example, 0, sizeof(spatem_example));

    configureUdp(&sockfd);

    printf("\nPreparing to decode the message...\n");

    int count = 0;
    while(count++ < 5) {
        
        int bytes_received = receiveMessage(sockfd, buffer, (struct sockaddr *)&client_addr, &addr_len);
        if (bytes_received == -1) {
            printf("Failed in message reception.\n");
        }
        printf("Bytes Received: %d.\n", bytes_received);

        if (decodeSPATEM(buffer, decoded_spatem) != 0) {
            printf("Failed to decode SPATEM message.\n");
        }

        printSPATEM(decoded_spatem);

        sleep(1);
    }

    close(sockfd);

    return 0;
}



void configureUdp(int *sockfd){
    
    struct sockaddr_in server_addr;

    // Create socket
    if ((*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind socket to port
    if (bind(*sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
}

int receiveMessage(int sockfd, char *buffer, struct sockaddr *client_addr, socklen_t *addr_len) {
    int bytes_received = recvfrom(sockfd, buffer, MAX_MSG_SIZE, 0, client_addr, addr_len);
    if (bytes_received == -1) {
        perror("Receive failed");
        return -1;
    }
    return bytes_received;
}

int decodeSPATEM(char *buffer, SPATEM_t *decoded_spatem){
    asn_dec_rval_t dc; /*Decoder return value*/
    uint16_t index = 1000;

    dc = uper_decode_complete(NULL, &asn_DEF_SPATEM, (void **)&decoded_spatem, buffer, index);

    if (dc.code != RC_OK) {
        printf("Error decoding buffer at bit %ld \n", dc.consumed);
        return -1;
    }

    return 0;
}

void printSPATEM(SPATEM_t *decoded_spatem){
    printf("ITS PDU Header: protocol Version: %ld, messageID: %ld, stationID: %ld\n",
            decoded_spatem->header.protocolVersion, decoded_spatem->header.messageID,
            decoded_spatem->header.stationID);

    if(decoded_spatem->spat.intersections.list.array)
    {
        printf("SPAT payload: timeStamp: %ld, intersection ID: %ld, intersection Name: %*s\n",
            *(decoded_spatem->spat.timeStamp), decoded_spatem->spat.intersections.list.array[0]->id.id,
            (int)decoded_spatem->spat.intersections.list.array[0]->name->size,
            decoded_spatem->spat.intersections.list.array[0]->name->buf);
    }
}
