#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "../inc/main.h"
#include "../asn1-header/SPATEM.h"
#include "../asn1-header/per_encoder.h"
#include "../asn1-header/per_decoder.h"
#include "../inc/message.h"

int createSocket();
void configureServerAddress(struct sockaddr_in *server_addr);
void sendMessage(int sockfd, struct sockaddr_in *server_addr, char *buffer, uint16_t bytes_enc);

#define SERVER_IP "192.168.1.5"
#define PORT 12345
#define MAX_MSG_SIZE 10000

int main(int argc, char **argv)
{
    MinuteOfTheYear_t currTimeSpat;
    uint8_t error = 0;
    uint8_t out_buffer[MAX_MSG_SIZE] = {0};
    uint16_t bytes_enc = 0;

    struct sockaddr_in server_addr;
    int sockfd = createSocket();
    configureServerAddress(&server_addr);

    error |= messageInit(&spatMessage, &intersectionArray, out_buffer);

    currTimeSpat = getCurrTime();
    spatMessage.spat.timeStamp = &currTimeSpat;

    printHeader(&spatMessage);

    printIntersections(&spatMessage);

    // ----------/ ENCODE ------------

    error |= encodeBuffer(&spatMessage, out_buffer, sizeof(out_buffer), &bytes_enc);
    memset(&spatMessage, 0, sizeof(spatMessage));

    // --------------------- ENCODE /-------------------

    // --------------------/ DECODE --------------------

    error |= decodeBuffer(decoded_spatem, intersectionArray, out_buffer, bytes_enc);

    printHeader(decoded_spatem);
    // ---------- DECODE /------------

    printIntersections(decoded_spatem);

    // ----------/ CODIGO TESTE ----------------
    int count = 0;
    while(count++ < 30) 
    {
        sendMessage(sockfd, &server_addr, out_buffer, bytes_enc);
        sleep(1);
    }

    // ---------- CODIGO TESTE /----------------

    close(sockfd);

    /* Free allocated memory */
    freeMemory();
    free(intersectionArray);

    return 0;
}

/* Socket creation */
int createSocket()
{
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

/* Configure server address */
void configureServerAddress(struct sockaddr_in *server_addr)
{
    memset(server_addr, 0, sizeof(*server_addr));

    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &(server_addr->sin_addr)) <= 0) {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }
}

/* Send message to server */
void sendMessage(int sockfd, struct sockaddr_in *server_addr, char *buffer, uint16_t bytes_enc)
{
    ssize_t sent_size = sendto(sockfd, buffer, bytes_enc, MSG_CONFIRM, (const struct sockaddr *) server_addr, sizeof(*server_addr));
    if (sent_size < 0) {
        perror("Failed to send message");
        exit(EXIT_FAILURE);
    }
    DEBUG_PRINT("Message sent\n");
}