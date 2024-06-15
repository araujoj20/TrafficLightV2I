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
#include "../inc/fsm.h"

int main(int argc, char **argv)
{
    MinuteOfTheYear_t currTimeSpat;
    uint8_t out_buffer[MAX_MSG_SIZE] = {0};
    uint16_t bytes_enc = 0;
    int sockfd = 0;
    struct sockaddr_in broadcast_addr;

    // ----------/ UDP ------------
    createSocket(&sockfd);
    configureBroadcast(&broadcast_addr);
    // ----------- UDP /-----------

    // --------------------/ MESSAGE --------------------
    //memset(&spatMessage, 0, sizeof(spatMessage));
    messageInit(&spatMessage, &intersectionArray, out_buffer, &currTimeSpat);
    // --------------------- MESSAGE /-------------------

    // ----------/ CODIGO TESTE ----------------

    setup();
    unsigned char f_priority = 0;
    unsigned int currentTime = 0;
    unsigned int lastTime = 0;
    unsigned int waitTime = 0;
    State currentState  = STATE_GREEN;
    State nextState     = STATE_GREEN;


    /*  Main Program Loop
            
        To do:
        - Cant always change the states of emergency and pedestrian !!!
        - fsm.c, all the states need extra code to work as it should
    */
    while(1){

        currentTime = millis();
        
        /*  Emergency Vehicle Detected */
        if (emergencyBtn) {
            currentState = STATE_EMERGENCY;
            emergencyBtn = false;
            f_priority = 1;
        }
        /*  Pedestrian Detected */
        else if (pedestrianBtn) {
            currentState = STATE_PEDESTRIAN;
            pedestrianBtn = false;
            f_priority = 1;
        }
    
        /*  States Change if nothing happen ( Green - Yellow - Red ) */
        if ((currentTime - lastTime >= waitTime) || f_priority) {
            DEBUG_PRINT("Current Time: %d\n", currentTime);
            DEBUG_PRINT("State: %d\n", currentState);
            nextState = operations[currentState].nextStateFunction();
            waitTime  = operations[currentState].time;
            lastTime  = currentTime;
            currentState = nextState;
            f_priority = 0;
        }
        
        /*  Encode and Create Message every 1s 
            
            Not done yet, still in thinking progress
            - Message Counter only increments when message has some change
            - Same as above, only encode if message has a change
        */
        if ((currentTime - lastTime >= 1000)) {
            lastTime  = currentTime;            
            currentState = (currentState + 1) % STATES_NUMBER;

            M_SET_CURR_TIME(currTimeSpat);
            M_SET_INTERSECTION(intersectionArray, 2);
            M_INCREMENT_COUNT(intersectionArray);
            M_SET_STATE(intersectionArray, currentState);
            
            encodeBuffer(&spatMessage, out_buffer, sizeof(out_buffer), &bytes_enc);
            printMessage(&spatMessage);
            
            sendMessage(sockfd, &broadcast_addr, out_buffer, bytes_enc);
        }
        usleep(1000);
    }
    // ---------- CODIGO TESTE /----------------

    /* Close Socket */
    close(sockfd);

    /* Free allocated memory */
    freeMemory();
    free(intersectionArray);

    return 0;
}