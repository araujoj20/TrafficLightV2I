#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "../inc/main.h"
#include "../asn1-header/SPATEM.h"
#include "../asn1-header/per_encoder.h"
#include "../asn1-header/per_decoder.h"
#include "../inc/message.h"
#include "../inc/udp.h"

#define SERVER_IP "192.168.1.5"
#define PORT 12345
#define MAX_MSG_SIZE 10000

int main(int argc, char **argv)
{
    MinuteOfTheYear_t currTimeSpat;
    uint8_t error = 0;
    uint8_t out_buffer[MAX_MSG_SIZE] = {0};
    uint16_t bytes_enc = 0;
    struct sockaddr_in broadcast_addr;

    // ----------/ UDP ------------
    int sockfd = createSocket();
    configureBroadcast(&broadcast_addr);
    // ----------- UDP /-----------

    error |= messageInit(&spatMessage, &intersectionArray, out_buffer);

    currTimeSpat = getCurrTime();
    spatMessage.spat.timeStamp = &currTimeSpat;

    printHeader(&spatMessage);
    printIntersections(&spatMessage);

    // --------------------/ ENCODE --------------------

    error |= encodeBuffer(&spatMessage, out_buffer, sizeof(out_buffer), &bytes_enc);
    memset(&spatMessage, 0, sizeof(spatMessage));

    // --------------------- ENCODE /-------------------

    // --------------------/ DECODE --------------------

    error |= decodeBuffer(decoded_spatem, intersectionArray, out_buffer, bytes_enc);

    printHeader(decoded_spatem);
    // --------------------- DECODE /-------------------

    printIntersections(decoded_spatem);

    // ----------/ CODIGO TESTE ----------------

    int count = 0;
    while(count++ < 30) 
    {
        sendMessage(sockfd, &broadcast_addr, out_buffer, bytes_enc);
        sleep(1);
    }

    // ---------- CODIGO TESTE /----------------

    close(sockfd);

    /* Free allocated memory */
    freeMemory();
    free(intersectionArray);

    return 0;
}