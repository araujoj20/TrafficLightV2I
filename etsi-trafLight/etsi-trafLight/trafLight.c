#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "asn1-header/SPATEM.h"
#include "asn1-header/per_encoder.h"
#include "asn1-header/per_decoder.h"

/* Prototypes */

void genHeader(SPATEM_t* spatMessage);
void genIntersection(IntersectionState_t* intersection, int int_index);
void genIntersectionStatus(IntersectionState_t* intersection, int intStatus);
void getStateAndTime(MovementEvent_t* event);
MinuteOfTheYear_t getCurrTime(void);

void printIntersectionInfo(const IntersectionState_t* intersection);
void freeMemory(void);

int createSocket();
void configureServerAddress(struct sockaddr_in *server_addr);
void sendMessage(int sockfd, struct sockaddr_in *server_addr, char *buffer, unsigned int bytes_enc);

/* Defines */

#define STATION_ID 0x12345678      
#define MAX_OF_INTERSECTIONS 4    
#define NUM_OF_INTERSECTIONS 4         

#define INTERSECTION_ID1        0x1111
#define INTERSECTION_ID2        0x2222
#define INTERSECTION_ID3        0x3333
#define INTERSECTION_ID4        0x4444

#define STATUS_1 IntersectionStatusObject_failureFlash                  // 2
#define STATUS_2 IntersectionStatusObject_signalPriorityIsActive        // 4
#define STATUS_3 IntersectionStatusObject_trafficDependentOperation     // 6
#define STATUS_4 IntersectionStatusObject_failureMode                   // 8

#define SERVER_IP "192.168.1.5"
#define PORT 12345
#define MAX_MSG_SIZE 10000       

/* Global Variables */

SPATEM_t spatMessage;
IntersectionID_t intersectionIDs[MAX_OF_INTERSECTIONS] = {INTERSECTION_ID1, INTERSECTION_ID2, INTERSECTION_ID3, INTERSECTION_ID4};
e_IntersectionStatusObject intersectionSTATUS[MAX_OF_INTERSECTIONS] = {STATUS_1, STATUS_2, STATUS_3, STATUS_4};
const char* intersectionNames[MAX_OF_INTERSECTIONS] = {"First INT", "Second INT", "Third INT", "Fourth INT"};

/* Functions */

int main(int argc, char **argv)
{
    asn_enc_rval_t ec; // Encoder return value
    asn_dec_rval_t dc; // Decoder return value

    uint16_t index = 0;
    int ret;
    uint8_t out_buffer[MAX_MSG_SIZE] = {0};
    unsigned int bytes_enc = 0;

    struct sockaddr_in server_addr;
    int sockfd = createSocket();
    configureServerAddress(&server_addr);

    /* HEADER */
    genHeader(&spatMessage);

    /* Allocate memory for the intersection array */
    IntersectionState_t* intersectionArray = (IntersectionState_t*) malloc(NUM_OF_INTERSECTIONS * sizeof(IntersectionState_t));
    if (intersectionArray == NULL) {
        printf("Memory allocation failed - IntersectionState_t*\n");
        return -1;
    }

    /* Allocate memory for the array of the intersections list */
    spatMessage.spat.intersections.list.array = (IntersectionState_t**) malloc(NUM_OF_INTERSECTIONS * sizeof(IntersectionState_t*));
    if (spatMessage.spat.intersections.list.array == NULL) {
        printf("Memory allocation failed - spatMessage.spat.intersections.list.array\n");
        free(intersectionArray);
        return -1;
    }

    /* Initialize the array pointers */
    for (int i = 0; i < NUM_OF_INTERSECTIONS; i++) {
        spatMessage.spat.intersections.list.array[i] = &intersectionArray[i];
    }

    spatMessage.spat.intersections.list.count = NUM_OF_INTERSECTIONS;
    spatMessage.spat.intersections.list.size = NUM_OF_INTERSECTIONS * sizeof(IntersectionState_t*);

    /* Generate each intersection */
    for (int i = 0; i < NUM_OF_INTERSECTIONS; i++) {
        genIntersection(&intersectionArray[i], i);
    }

    /* Get the current time */
    MinuteOfTheYear_t currTimeSpat = getCurrTime();

    /* Time Stamp - Current time in MinuteOfTheYear_t format */
    spatMessage.spat.timeStamp = &currTimeSpat;


    ret = asn_check_constraints(&asn_DEF_IntersectionStateList, &spatMessage.spat.intersections.list, out_buffer, (size_t*)&index);
    if(ret)
    {
        printf("Error checking constraints - Intersection State List: %s\n", out_buffer);
        free(intersectionArray);
        free(spatMessage.spat.intersections.list.array);
        return -1;
    }

    ret = asn_check_constraints(&asn_DEF_SPATEM, &spatMessage, out_buffer, (size_t*)&index);
    if(ret)
    {
        printf("Error checking constraints - SPATEM: %s\n", out_buffer);
        free(intersectionArray);
        free(spatMessage.spat.intersections.list.array);
        return -1;
    }

    memset(out_buffer, 0, sizeof(out_buffer));


    printf("ITS PDU Header: protocol Version: %ld, messageID: %ld, stationID: %ld\n",
            spatMessage.header.protocolVersion, spatMessage.header.messageID,
            spatMessage.header.stationID);

    // if(spatMessage.spat.intersections.list.array)
    // {
    //     for(int i=0; i<NUM_OF_INTERSECTIONS; i++)
    //     {
    //     printf("SPAT payload: timeStamp: %ld, intersection ID: 0x%lx, intersection Name: %*s\n",
    //         *(spatMessage.spat.timeStamp), spatMessage.spat.intersections.list.array[i]->id.id,
    //         (int)spatMessage.spat.intersections.list.array[i]->name->size, 
    //         spatMessage.spat.intersections.list.array[i]->name->buf);
    //     }
    // }

    if(spatMessage.spat.intersections.list.array)
    {
        for (int i = 0; i < NUM_OF_INTERSECTIONS; i++) 
        {
            printIntersectionInfo(spatMessage.spat.intersections.list.array[i]);
        }
    }

    printf("\nPreparing to encode the message...\n");

    ec = uper_encode_to_buffer(&asn_DEF_SPATEM, NULL, &spatMessage, out_buffer, sizeof(out_buffer));

    bytes_enc = (ec.encoded + 7) / 8;

    if (ec.encoded > 0)
    {
        index = 0;
        printf("Bits enconded: %ld\n", ec.encoded);
        while (index != bytes_enc)
        {
            printf("0x%02x ", out_buffer[index]);
            index++;
        }
        printf("\n");
    }
    else
    {
        printf("Error while encoding: %s\n", ec.failed_type ? ec.failed_type->name : "unknown");
    }

    SPATEM_t* decoded_spatem = &spatMessage;

    memset(&spatMessage, 0, sizeof(spatMessage));

    printf("\nPreparing to decode the message...\n");

    dc = uper_decode_complete(NULL, &asn_DEF_SPATEM, (void **)&decoded_spatem, out_buffer, index);

    if (dc.code != RC_OK)
    {
        printf("Error decoding buffer at bit %ld \n", dc.consumed);
        free(intersectionArray);
        free(spatMessage.spat.intersections.list.array);
        return -1;
    }

    printf("Message decoded with success\n");

    printf("ITS PDU Header: protocol Version: %ld, messageID: %ld, stationID: %lx\n",
            decoded_spatem->header.protocolVersion, decoded_spatem->header.messageID,
            decoded_spatem->header.stationID);

    if(decoded_spatem->spat.intersections.list.array)
    {
        for(int i=0; i<NUM_OF_INTERSECTIONS; i++)
        {
        // printf("SPAT payload: timeStamp: %ld, intersection ID: 0x%lx, intersection Name: %*s\n",
        //     *(decoded_spatem->spat.timeStamp), decoded_spatem->spat.intersections.list.array[i]->id.id,
        //     (int)decoded_spatem->spat.intersections.list.array[i]->name->size, 
        //     decoded_spatem->spat.intersections.list.array[i]->name->buf);

            printIntersectionInfo(decoded_spatem->spat.intersections.list.array[i]);
        }
    }

    int count = 0;
    while(count++ < 30) {
        
        sendMessage(sockfd, &server_addr, out_buffer, bytes_enc);

        sleep(1);
    }

    close(sockfd);

    /* Free allocated memory */
    freeMemory();
    free(intersectionArray);

    return 0;
}

/* -------------------------------------- DF and DE --------------------------------------*/

/* Generate the SPATEM Header */
void genHeader(SPATEM_t* spatMessage)
{
    /* Protocol Version - version of the ITS payload contained in the message */
    spatMessage->header.protocolVersion = 1;

    /* Message ID - type of the ITS payload contained in the message */
    spatMessage->header.messageID = ItsPduHeader__messageID_spatem;

    /* Station ID - unique identifier of the ITS-S originating the message */
    spatMessage->header.stationID = STATION_ID;
}

/* Generate an intersection */
void genIntersection(IntersectionState_t* intersection, int int_index)
{
    int i = int_index;
    if (i < 0 || i >= NUM_OF_INTERSECTIONS) {
        return;
    }

    /* Intersection ID */
    intersection->id.id = intersectionIDs[i];

    /* Intersection name */
    intersection->name = (DescriptiveName_t*) malloc(sizeof(DescriptiveName_t));
    if (intersection->name == NULL) {
        printf("Memory allocation failed - Intersection Name\n");
        exit(-1);
    }
    size_t name_len = strlen(intersectionNames[i]);
    intersection->name->buf = (uint8_t*) malloc(name_len);
    if (intersection->name->buf == NULL) {
        printf("Memory allocation failed - Intersection Name Buffer\n");
        exit(-1);
    }
    memcpy(intersection->name->buf, intersectionNames[i], name_len);
    intersection->name->size = name_len;

    /* Generate the status */
    genIntersectionStatus(intersection, intersectionSTATUS[i]);

    /* Generate the list of states */
    intersection->states.list.count = 1;
    intersection->states.list.size = 1 * sizeof(MovementState_t*);
    intersection->states.list.array = (MovementState_t**) malloc(1 * sizeof(MovementState_t*));
    if (intersection->states.list.array == NULL) {
        printf("Memory allocation failed - States List Array\n");
        exit(-1);
    }
    intersection->states.list.array[0] = (MovementState_t*) malloc(sizeof(MovementState_t));
    if (intersection->states.list.array[0] == NULL) {
        printf("Memory allocation failed - State List Array Element\n");
        exit(-1);
    }

    /* Initialize the state */
    intersection->states.list.array[0]->state_time_speed.list.array = (MovementEvent_t**) malloc(sizeof(MovementEvent_t*));
    if (intersection->states.list.array[0]->state_time_speed.list.array == NULL) {
        printf("Memory allocation failed - State Time Speed List Array\n");
        exit(-1);
    }
    intersection->states.list.array[0]->state_time_speed.list.count = 1;
    intersection->states.list.array[0]->state_time_speed.list.size = sizeof(MovementEvent_t*);
    intersection->states.list.array[0]->state_time_speed.list.array[0] = (MovementEvent_t*) malloc(sizeof(MovementEvent_t));
    if (intersection->states.list.array[0]->state_time_speed.list.array[0] == NULL) {
        printf("Memory allocation failed - State Time Speed List Array Element\n");
        exit(-1);
    }

    getStateAndTime(&intersection->states.list.array[0]->state_time_speed.list.array[0]);
}

/* Generate the status of an intersection */
void genIntersectionStatus(IntersectionState_t* intersection, int intStatus)
{
    intersection->status.buf = (uint8_t*) malloc(1);
    if (intersection->status.buf == NULL) {
        printf("Memory allocation failed - Intersection Status Buffer\n");
        exit(-1);
    }
    intersection->status.buf[0] = intStatus;
    intersection->status.size = 1;
}

/* Generate state and timing */
void getStateAndTime(MovementEvent_t* event)
{
    /* Timing information */
    event->timing = (TimeChangeDetails_t*) malloc(sizeof(TimeChangeDetails_t));
    if (event->timing == NULL) {
        printf("Memory allocation failed - Event Timing\n");
        exit(-1);
    }

    event->timing->startTime = (MinuteOfTheYear_t*) malloc(sizeof(MinuteOfTheYear_t));
    if (event->timing->startTime == NULL) {
        printf("Memory allocation failed - StartTime\n");
        exit(-1);
    }
    *(event->timing->startTime) = getCurrTime();

    event->timing->minEndTime = *(event->timing->startTime) + 1;

    event->timing->maxEndTime = (MinuteOfTheYear_t*) malloc(sizeof(MinuteOfTheYear_t));
    if (event->timing->maxEndTime == NULL) {
        printf("Memory allocation failed - MaxEndTime\n");
        exit(-1);
    }
    *(event->timing->maxEndTime) = *(event->timing->startTime) + 2;
}

/* Get the current time in MinuteOfTheYear_t format */
MinuteOfTheYear_t getCurrTime(void)
{
    return 123456;
}

void printIntersectionInfo(const IntersectionState_t* intersection) {
    if (intersection == NULL) {
        printf("Intersection is NULL\n");
        return;
    }

    printf("Intersection ID: 0x%lx\n", intersection->id.id);
    
    if (intersection->name != NULL) {
        printf("Intersection Name: %.*s\n", (int)intersection->name->size, intersection->name->buf);
    } else {
        printf("Intersection Name: NULL\n");
    }

    if (intersection->status.buf != NULL) {
        printf("Intersection Status: 0x%02x\n", intersection->status.buf[0]);
    } else {
        printf("Intersection Status: NULL\n");
    }

    printf("Number of States: %ld\n", intersection->states.list.count);

    for (int i = 0; i < intersection->states.list.count; i++) {
        MovementState_t* state = intersection->states.list.array[i];
        if (state == NULL) {
            printf("State %d is NULL\n", i);
            continue;
        }

        printf("  State %d:\n", i);

        printf("    Number of Events: %ld\n", state->state_time_speed.list.count);

        for (int j = 0; j < state->state_time_speed.list.count; j++) {
            MovementEvent_t* event = state->state_time_speed.list.array[j];
            if (event == NULL) {
                printf("    Event %d is NULL\n", j);
                continue;
            }

            printf("    Event %d:\n", j);

            if (event->timing != NULL) {
                printf("      Start Time: %ld\n", *(event->timing->startTime));
                printf("      Min End Time: %ld\n", event->timing->minEndTime);
                if (event->timing->maxEndTime != NULL) {
                    printf("      Max End Time: %ld\n", *(event->timing->maxEndTime));
                } else {
                    printf("      Max End Time: NULL\n");
                }
            } else {
                printf("      Timing: NULL\n");
            }
        }
    }
}


void freeMemory(void)
{
    for (int i = 0; i < NUM_OF_INTERSECTIONS; i++) 
    {
        if (spatMessage.spat.intersections.list.array[i]->name) 
        {
            free(spatMessage.spat.intersections.list.array[i]->name->buf);
            free(spatMessage.spat.intersections.list.array[i]->name);
        }
        
        for (int j = 0; j < spatMessage.spat.intersections.list.array[i]->states.list.count; j++) 
        {
            for (int k = 0; k < spatMessage.spat.intersections.list.array[i]->states.list.array[j]->state_time_speed.list.count; k++) 
            {
                if (spatMessage.spat.intersections.list.array[i]->states.list.array[j]->state_time_speed.list.array[k]->timing) 
                {
                    if (spatMessage.spat.intersections.list.array[i]->states.list.array[j]->state_time_speed.list.array[k]->timing->startTime)
                    {
                        free(spatMessage.spat.intersections.list.array[i]->states.list.array[j]->state_time_speed.list.array[k]->timing->startTime);
                    }
                    if (spatMessage.spat.intersections.list.array[i]->states.list.array[j]->state_time_speed.list.array[k]->timing->maxEndTime)
                    {
                        free(spatMessage.spat.intersections.list.array[i]->states.list.array[j]->state_time_speed.list.array[k]->timing->maxEndTime);
                    }
                    free(spatMessage.spat.intersections.list.array[i]->states.list.array[j]->state_time_speed.list.array[k]->timing);
                }
                free(spatMessage.spat.intersections.list.array[i]->states.list.array[j]->state_time_speed.list.array[k]);
            }
            free(spatMessage.spat.intersections.list.array[i]->states.list.array[j]->state_time_speed.list.array);
            free(spatMessage.spat.intersections.list.array[i]->states.list.array[j]);
        }
        free(spatMessage.spat.intersections.list.array[i]->states.list.array);
        if (spatMessage.spat.intersections.list.array[i]->status.buf)
        {
            free(spatMessage.spat.intersections.list.array[i]->status.buf);
        }
    }
    free(spatMessage.spat.intersections.list.array);
}


/* -------------------------------------- SERVER --------------------------------------*/

int createSocket()
{
    int sockfd;

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

void configureServerAddress(struct sockaddr_in *server_addr)
{
    memset(server_addr, 0, sizeof(*server_addr));

    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(PORT);
    server_addr->sin_addr.s_addr = inet_addr(SERVER_IP);
}

void sendMessage(int sockfd, struct sockaddr_in *server_addr, char *buffer, unsigned int bytes_enc)
{
    int n;
    socklen_t len = sizeof(*server_addr);

    n = sendto(sockfd, (const char *)buffer, bytes_enc, MSG_CONFIRM, (const struct sockaddr *) server_addr, len);

    if (n < 0)
    {
        perror("Failed to send message");
        return;
    }

    printf("Sent %d bytes to the server\n", n);
}