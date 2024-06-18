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
    MinuteOfTheYear_t currTimeSpat = 0;
    uint8_t buffer[MAX_MSG_SIZE] = {0};
    uint16_t bytes_enc = 0;
    int sockfd = 0;
    struct sockaddr_in broadcast_addr;

    IntersectionData interData = {
        .intersectionIDs    = {INTER_ID1, INTER_ID2, INTER_ID3, INTER_ID4},
        .intersectionSTATUS = {INTER_STATUS1, INTER_STATUS2, INTER_STATUS3, INTER_STATUS4},
        .intersectionNames  = {INTER_NAME1, INTER_NAME2, INTER_NAME3, INTER_NAME4}
    };

    // ----------/ UDP ------------
    createSocket(&sockfd);
    configureBroadcast(&broadcast_addr);
    // ----------- UDP /-----------

    // --------------------/ MESSAGE --------------------
    //memset(&spatMessage, 0, sizeof(spatMessage));
    spatMessage.spat.timeStamp = &currTimeSpat;
    currTimeSpat = 100;
    messageInit(&spatMessage, &intersectionArray, &interData, buffer, &currTimeSpat);
    // --------------------- MESSAGE /-------------------
    
    // ----------/ CODIGO TESTE ----------------

    setup();
    unsigned char f_priority = 0;
    unsigned char f_emergency = 0;
    unsigned char f_pedestrianWaiting = 0;
    unsigned int currentTime = 0;
    unsigned int waitTime = 0;
    State currentState  = STATE_GREEN;
    State nextState     = STATE_GREEN;
    State lastState     = STATE_RED;

    /*  Main Program Loop
            
        To do:
        - Cant always change the states of emergency and pedestrian !!!
        - fsm.c, all the states need extra code to work as it should
    */

    unsigned int miliSecOfHour = getMillisecondsOfHour();
    unsigned int firstTime = millis();
    unsigned int stateLastTime = firstTime;
    unsigned int messageLastTime = firstTime;
    unsigned int emergencyLastTime = firstTime;
    unsigned int startTime = firstTime;
    
    while(1){

        currentTime = millis() - firstTime;
        
        /*  Emergency Vehicle Detected */
        if (emergencyBtn || (f_emergency && (currentTime - emergencyLastTime >= 500))) {
            
            if (f_emergency == 0){
                emergencyBtn = false;
                stateLastTime = currentTime;
                f_emergency = 1;
                startTime = currentTime;
            }
            else if (f_emergency && currentTime > stateLastTime){
                f_emergency = 0;
                stateLastTime = currentTime;
            } 
            else{
                currentState = operations[STATE_EMERGENCY].stateFunction();
                emergencyLastTime = currentTime;
            }
        }
        /*  Pedestrian Detected */
        else if (pedestrianBtn || f_pedestrianWaiting) {
            
            if ((currentState != STATE_EMERGENCY) && (currentTime >= operations[currentState].timeMin)){
                
                currentState = STATE_PEDESTRIAN;
                f_priority = 1;
                f_pedestrianWaiting = 0;
            }
            else{
                f_pedestrianWaiting = 1;
            }
            pedestrianBtn = false;
        }

        /*  States Change if nothing happen ( Green - Yellow - Red ) */
        if ((currentTime - stateLastTime >= waitTime) || f_priority ) {
            DEBUG_PRINT("Current Time: %d\n", currentTime);
            DEBUG_PRINT("State: %d\n", currentState);
            nextState = operations[currentState].stateFunction();
            waitTime  = operations[currentState].time;
            startTime = currentTime;
            stateLastTime  = currentTime;
            currentState = nextState;
            f_priority = 0;
        }
        
        /*  Encode and Create Message every 1s 
            
            Not done yet, still in thinking progress
            - Message Counter only increments when message has some change
            - Same as above, only encode if message has a change
        */

        
        if ((currentTime - messageLastTime >= 1000)) {
            
            if (lastState != currentState){

                M_SET_INTERSECTION(intersectionArray, 0);
                M_INCREMENT_COUNT(intersectionArray);
                M_SET_STATE(intersectionArray, currentState);
                M_SET_CURR_TIME(currTimeSpat);
                M_SET_MAX_TIME(intersectionArray, (miliSecOfHour + operations[currentState].time)/100);
                M_SET_MIN_TIME(intersectionArray, (miliSecOfHour + operations[currentState].timeMin)/100);
                M_SET_START_TIME(intersectionArray, (miliSecOfHour + startTime)/100);
                memset(buffer, 0, sizeof(buffer));
                encodeBuffer(&spatMessage, buffer, sizeof(buffer), &bytes_enc);
                sendMessage(sockfd, &broadcast_addr, buffer, bytes_enc);
                //printMessage(&spatMessage);
            }
            else{
                sendMessage(sockfd, &broadcast_addr, buffer, bytes_enc);
            }
            
            messageLastTime  = currentTime;            
        }

        usleep(1000);
        //DEBUG_PRINT("Time: %d\n", currentTime);
    }
    // ---------- CODIGO TESTE /----------------

    /* Close Socket */
    close(sockfd);

    /* Free allocated memory */
    freeMemory(&spatMessage);
    free(intersectionArray);

    return 0;
}