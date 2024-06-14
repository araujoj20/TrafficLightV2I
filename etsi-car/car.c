#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../inc/display.h"

#include "asn1-header/SPATEM.h"
#include "asn1-header/per_decoder.h"

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

    printf("\nWaiting for messages...\n");

    ssd1306_begin(SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
	ssd1306_clearDisplay();

    while(1) {
        
        int bytes_received = receiveMessage(sockfd, buffer, (struct sockaddr *)&client_addr, &addr_len);
        if (bytes_received == -1) {
            printf("Failed in message reception.\n");
        }
        printf("Bytes Received: %d.\n", bytes_received);

        if (decodeSPATEM(buffer, decoded_spatem) != 0) {
            printf("Failed to decode SPATEM message.\n");
        }

        printSPATEM(decoded_spatem);
        printDisplay(decoded_spatem);
    }

    close(sockfd);

    return 0;
}