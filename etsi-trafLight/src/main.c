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
    volatile unsigned char f_priority = 0;
    volatile unsigned char f_emergency = 0;
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
    unsigned int stateLastTime = 0;
    unsigned int messageLastTime = 0;
    unsigned int emergencyLastTime = 0;
    unsigned int startTime = 0;
    
    while(1){

        currentTime = millis() - firstTime;
        
        /*  Emergency Vehicle Detected */
        if (emergencyBtn){
            
            if (f_emergency == 0){
                DEBUG_PRINT("EMERGENCY PRESSED\n");
                //printStats(currentState,currentTime,stateLastTime,messageLastTime,emergencyLastTime,startTime);
                emergencyBtn = 0;
                stateLastTime = currentTime;
                f_emergency = 1;
                startTime = currentTime;
                currentState = STATE_EMERGENCY;
            }
        }
        if(f_emergency){
            
            if (f_emergency && ((currentTime - stateLastTime) > operations[STATE_EMERGENCY].time)){
                DEBUG_PRINT("EMERGENCY OUT\n");
                //printStats(currentState,currentTime,stateLastTime,messageLastTime,emergencyLastTime,startTime);
                f_emergency = 0;
                stateLastTime = 0;
                currentState = nextState;
                emergencyLastTime = 0;
            } 
            else if (f_emergency && (currentTime - emergencyLastTime >= 500)){ 
                DEBUG_PRINT("EMERGENCY FUNCTION CALL\n");
                //printStats(currentState,currentTime,stateLastTime,messageLastTime,emergencyLastTime,startTime);
                nextState = operations[STATE_EMERGENCY].stateFunction();
                emergencyLastTime = currentTime;
            }
        }
        /*  Pedestrian Detected */
        else if (pedestrianBtn || f_pedestrianWaiting) {
            
            if ((currentState != STATE_EMERGENCY) && (currentTime - stateLastTime >= operations[currentState].timeMin)){
                
                DEBUG_PRINT("PEDESTRIAN STATE\n");
                //printStats(currentState,currentTime,stateLastTime,messageLastTime,emergencyLastTime,startTime);
                
                currentState = STATE_PEDESTRIAN;
                f_priority = 1;
                f_pedestrianWaiting = 0;
            }
            else{
                if (!f_pedestrianWaiting){
                    DEBUG_PRINT("PEDESTRIAN BUGG STATE\n");
                    //printStats(currentState,currentTime,stateLastTime,messageLastTime,emergencyLastTime,startTime);
                    f_pedestrianWaiting = 1;
                }
            }
            pedestrianBtn = 0;
        }

        /*  States Change if nothing happen ( Green - Yellow - Red ) */
        else if ((currentTime - stateLastTime >= waitTime) || f_priority ) {
            
            if (f_priority == 0)
                currentState = nextState;
            DEBUG_PRINT("STATE MACHINE\n");
            DEBUG_PRINT("Current Time: %d\n", currentTime);
            //printStats(currentState,currentTime,stateLastTime,messageLastTime,emergencyLastTime,startTime);

            nextState = operations[currentState].stateFunction();
            waitTime  = operations[currentState].time;
            startTime = currentTime;
            stateLastTime  = currentTime;
            f_priority = 0;
        }
        
        /*  Encode and Create Message every 1s 
            
            Not done yet, still in thinking progress
            - Message Counter only increments when message has some change
            - Same as above, only encode if message has a change
        */

        
        if ((currentTime - messageLastTime >= 1000)) {
            
            static unsigned int m_startTime = 0;
            static State m_state = 20;

            if ((m_startTime != startTime || m_state != currentState)){
                m_startTime = startTime;
                m_state = currentState;
                DEBUG_PRINT("Current Time: %d\n", currentTime);
                M_SET_INTERSECTION(intersectionArray, 0);
                M_INCREMENT_COUNT(intersectionArray);
                M_SET_STATE(intersectionArray, currentState);
                M_SET_CURR_TIME(currTimeSpat);
                M_SET_MAX_TIME(intersectionArray, (miliSecOfHour + startTime + operations[currentState].time)/100);
                M_SET_MIN_TIME(intersectionArray, (miliSecOfHour + startTime + operations[currentState].timeMin)/100);
                M_SET_START_TIME(intersectionArray, (miliSecOfHour + startTime)/100);
                memset(buffer, 0, sizeof(buffer));
                encodeBuffer(&spatMessage, buffer, sizeof(buffer), &bytes_enc);
                //sendMessage(sockfd, &broadcast_addr, buffer, bytes_enc);
                
                //printStats(currentState,currentTime,stateLastTime,messageLastTime,emergencyLastTime,startTime);
            }
            else{
                sendMessage(sockfd, &broadcast_addr, buffer, bytes_enc);
                printMessage(&spatMessage);
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


const char *stateNames[STATES_NUMBER] = {
    "STATE_GREEN",
    "STATE_YELLOW",
    "STATE_RED",
    "STATE_PEDESTRIAN",
    "STATE_EMERGENCY"
};

void printStats(State currentState, unsigned int currentTime, unsigned int stateLastTime, 
unsigned int messageLastTime, unsigned int emergencyLastTime, unsigned int startTime){

    if (currentState >= 0 && currentState < STATES_NUMBER) {
        printf("State: %s\n", stateNames[currentState]);
    } else {
        printf("State: Unknown\n");
    }

    printf("currentTime      : %d\n", currentTime );
    printf("stateLastTime    : %d\n", stateLastTime);
    printf("messageLastTime  : %d\n", messageLastTime);
    printf("emergencyLastTime: %d\n", emergencyLastTime);
    printf("startTime        : %d\n", startTime);

}