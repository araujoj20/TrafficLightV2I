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
//#include "../inc/display.h"

int main(int argc, char **argv)
{
    MinuteOfTheYear_t currTimeSpat;
    uint8_t error = 0;
    uint8_t buffer[MAX_MSG_SIZE] = {0};
    uint16_t bytes_enc = 0;
    int sockfd = 0;
    struct sockaddr_in broadcast_addr;
    socklen_t addr_len = sizeof(broadcast_addr);

    // ----------/ UDP ------------
    createSocket(&sockfd);
    configureBroadcast(&broadcast_addr);
    rxBroadcast(&sockfd, &broadcast_addr, &addr_len);
    // ----------- UDP /-----------

    // currTimeSpat = getCurrTime();
    // spatMessage.spat.timeStamp = &currTimeSpat;

    // -----------/ DISPLAY -----------
    //ssd1306_begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
    //ssd1306_clearDisplay();
    // ------------ DISPLAY /-----------

    // --------------------/ DECODE --------------------

    // error |= decodeBuffer(decoded_spatem, intersectionArray, buffer, bytes_enc);

    // printHeader(decoded_spatem);
    // printIntersections(decoded_spatem);
    // --------------------- DECODE /-------------------

    // ----------/ CODIGO TESTE ----------------

    while(1) {
        
        int bytes_received = receiveMessage(sockfd, buffer, (struct sockaddr *)&broadcast_addr, &addr_len);
        if (bytes_received == -1) {
            printf("Failed in message reception.\n");
        }
        printf("Bytes Received: %d.\n", bytes_received);

        error |= decodeBuffer(decoded_spatem, intersectionArray, buffer, bytes_received);

        printHeader(decoded_spatem);
        printIntersections(decoded_spatem);
        //printDisplay(decoded_spatem);
        printf("\033[1;31m -------------------------------------------------------------- \033[0m\n");
    }

    // ---------- CODIGO TESTE /----------------

    close(sockfd);

    /* Free allocated memory */
    freeMemory();
    free(intersectionArray);

    return 0;
}