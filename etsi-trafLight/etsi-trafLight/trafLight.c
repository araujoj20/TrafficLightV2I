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

#define INTERSECTION_ID1 0x1111
#define INTERSECTION_ID2 0x2222
#define INTERSECTION_ID3 0x3333
#define INTERSECTION_ID4 0x4444

#define STATUS_1 IntersectionStatusObject_failureFlash // 2
#define STATUS_2 IntersectionStatusObject_signalPriorityIsActive // 4
#define STATUS_3 IntersectionStatusObject_trafficDependentOperation // 6
#define STATUS_4 IntersectionStatusObject_failureMode // 8

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

    SPATEM_t* decoded_spatem = &spatMessage;

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

    printf("ITS PDU Header:\nProtocol Version: %ld     Message ID: %ld     Station ID: 0x%lx     Time: %ld\n",
            spatMessage.header.protocolVersion, spatMessage.header.messageID,
            spatMessage.header.stationID, *(spatMessage.spat.timeStamp));
    printf("-----------------------------------------------------------------------------------\n");

    if(spatMessage.spat.intersections.list.array) 
    {
        for (int i = 0; i < NUM_OF_INTERSECTIONS; i++) {
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
        while (index != bytes_enc) {
            printf("0x%02x ", out_buffer[index]);
            index++;
        }
        printf("\n");
    } else 
    {
        printf("Error while encoding: %s\n", ec.failed_type ? ec.failed_type->name : "unknown");
    }

    memset(&spatMessage, 0, sizeof(spatMessage));

    printf("\nPreparing to decode the message...\n");

    dc = uper_decode_complete(NULL, &asn_DEF_SPATEM, (void **)&decoded_spatem, out_buffer, index);
    if (dc.code != RC_OK) 
    {
        printf("Error decoding buffer. Bits decoded: %ld \n", dc.consumed);
        free(intersectionArray);
        free(spatMessage.spat.intersections.list.array);
        return -1;
    }



    printf("Message decoded with success\n");


    printf("ITS PDU Header:\nProtocol Version: %ld     Message ID: %ld     Station ID: 0x%lx     Time: %ld\n",
            decoded_spatem->header.protocolVersion, decoded_spatem->header.messageID,
            decoded_spatem->header.stationID, *(decoded_spatem->spat.timeStamp));
    printf("-----------------------------------------------------------------------------------\n");

    if(decoded_spatem->spat.intersections.list.array) 
    {
        for (int i = 0; i < NUM_OF_INTERSECTIONS; i++) {
            printIntersectionInfo(decoded_spatem->spat.intersections.list.array[i]);
        }
    }

    int count = 0;
    while(count++ < 30) 
    {
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
    intersection->name->buf = (uint8_t*) malloc(name_len + 1);
    if (intersection->name->buf == NULL) {
        printf("Memory allocation failed - Intersection Name Buffer\n");
        exit(-1);
    }
    memcpy(intersection->name->buf, intersectionNames[i], name_len);
    intersection->name->buf[name_len] = '\0';
    intersection->name->size = name_len;

    /* Intersection status */
    genIntersectionStatus(intersection, intersectionSTATUS[i]);

    /* Allocate and initialize MovementState_t array for 1 movement (for simplicity) */
    MovementState_t* movementArray = (MovementState_t*) malloc(1 * sizeof(MovementState_t));
    if (movementArray == NULL) {
        printf("Memory allocation failed - MovementState_t*\n");
        exit(-1);
    }
    intersection->states.list.array = (MovementState_t**) malloc(1 * sizeof(MovementState_t*));
    if (intersection->states.list.array == NULL) {
        printf("Memory allocation failed - intersection->states.list.array\n");
        free(movementArray);
        exit(-1);
    }
    intersection->states.list.array[0] = &movementArray[0];
    intersection->states.list.count = 1;
    intersection->states.list.size = 1 * sizeof(MovementState_t*);

    /* Initialize the MovementState_t array */
    movementArray[0].movementName = (DescriptiveName_t*) malloc(sizeof(DescriptiveName_t));
    if (movementArray[0].movementName == NULL) {
        printf("Memory allocation failed - Movement Name\n");
        exit(-1);
    }
    const char* movementName = "Sample Movement";
    size_t movementName_len = strlen(movementName);
    movementArray[0].movementName->buf = (uint8_t*) malloc(movementName_len + 1);
    if (movementArray[0].movementName->buf == NULL) {
        printf("Memory allocation failed - Movement Name Buffer\n");
        exit(-1);
    }
    memcpy(movementArray[0].movementName->buf, movementName, movementName_len);
    movementArray[0].movementName->buf[movementName_len] = '\0';
    movementArray[0].movementName->size = movementName_len;

    /* MovementEvent */
    MovementEvent_t* movementEventArray = (MovementEvent_t*) malloc(1 * sizeof(MovementEvent_t));
    if (movementEventArray == NULL) {
        printf("Memory allocation failed - MovementEvent_t*\n");
        exit(-1);
    }
    movementArray[0].state_time_speed.list.array = (MovementEvent_t**) malloc(1 * sizeof(MovementEvent_t*));
    if (movementArray[0].state_time_speed.list.array == NULL) {
        printf("Memory allocation failed - movementArray[0].state_time_speed.list.array\n");
        free(movementEventArray);
        exit(-1);
    }
    movementArray[0].state_time_speed.list.array[0] = &movementEventArray[0];
    movementArray[0].state_time_speed.list.count = 1;
    movementArray[0].state_time_speed.list.size = 1 * sizeof(MovementEvent_t*);

    /* Generate the MovementEvent_t */
    getStateAndTime(&movementEventArray[0]);
}

/* Generate the intersection status */
void genIntersectionStatus(IntersectionState_t* intersection, int intStatus)
{
    uint16_t i;
    intersection->status.buf = (uint8_t*) calloc(1, sizeof(uint8_t));
    if (intersection->status.buf == NULL) {
        printf("Memory allocation failed - intersection->status.buf\n");
        exit(-1);
    }

    intersection->status.size = 1;
    *intersection->status.buf = intStatus;
}

/* Get the state and time */
void getStateAndTime(MovementEvent_t* event)
{
    /* Set traffic light state */
    event->eventState = MovementPhaseState_stop_Then_Proceed;

    /* Allocate memory for the timing structure */
    event->timing = (TimeChangeDetails_t*) malloc(sizeof(TimeChangeDetails_t));
    if (event->timing == NULL) {
        printf("Memory allocation failed - TimeChangeDetails_t\n");
        exit(-1);
    }

    /* Set the startTime */
    event->timing->startTime = (TimeMark_t*) malloc(sizeof(TimeMark_t));
    if (event->timing->startTime == NULL) {
        printf("Memory allocation failed - startTime\n");
        exit(-1);
    }
    *(event->timing->startTime) = 10;

    /* Set the minEndTime */
    event->timing->minEndTime = 20;

    /* Set the maxEndTime */
    event->timing->maxEndTime = (TimeMark_t*) malloc(sizeof(TimeMark_t));
    if (event->timing->maxEndTime == NULL) {
        printf("Memory allocation failed - maxEndTime\n");
        exit(-1);
    }
    *(event->timing->maxEndTime) = 30;
}

/* Get the current time */
MinuteOfTheYear_t getCurrTime(void)
{
    return 200;
}

/* Print intersection information */
void printIntersectionInfo(const IntersectionState_t* intersection)
{
    if (!intersection) 
    {   
        printf("Error! Intersection is NULL\n");
        return;
    }

    MovementState_t* state = intersection->states.list.array[0];
    MovementEvent_t* event = state->state_time_speed.list.array[0];

    printf(" Intersection ID     : 0x%-13lx|\t", intersection->id.id);
    printf(" Traffic Light State : %ld\n", event->eventState);
    printf(" Intersection Name   : %-15s|\t", intersection->name->buf);
    printf(" Start Time          : %ld\n", *(event->timing->startTime));
    printf(" Intersection Status : 0x%02x           |\t", intersection->status.buf[0]);
    printf(" Min End Time        : %ld\n", event->timing->minEndTime);
    printf(" Message Count       : %-15ld|\t", intersection->revision);
    printf(" Max End Time        : %ld\n", *(event->timing->maxEndTime));
    printf("-----------------------------------------------------------------------------------\n");
}

/* Free dynamically allocated memory - supports more than one event or state*/
void freeMemory(void)
{
    for (int i = 0; i < NUM_OF_INTERSECTIONS; i++) {
        IntersectionState_t* intersection = spatMessage.spat.intersections.list.array[i];
        if (intersection->name) {
            if (intersection->name->buf) {
                free(intersection->name->buf);
            }
            free(intersection->name);
        }
        for (int j = 0; j < intersection->states.list.count; j++) {
            MovementState_t* movement = intersection->states.list.array[j];
            if (movement->movementName) {
                if (movement->movementName->buf) {
                    free(movement->movementName->buf);
                }
                free(movement->movementName);
            }
            for (int k = 0; k < movement->state_time_speed.list.count; k++) {
                MovementEvent_t* event = movement->state_time_speed.list.array[k];
                if (event->timing) {
                    if (event->timing->startTime) {
                        free(event->timing->startTime);
                    }
                    if (event->timing->maxEndTime) {
                        free(event->timing->maxEndTime);
                    }
                    free(event->timing);
                }
            }
            free(movement->state_time_speed.list.array);
        }
        free(intersection->states.list.array);
    }
    free(spatMessage.spat.intersections.list.array);
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
void sendMessage(int sockfd, struct sockaddr_in *server_addr, char *buffer, unsigned int bytes_enc)
{
    ssize_t sent_size = sendto(sockfd, buffer, bytes_enc, MSG_CONFIRM, (const struct sockaddr *) server_addr, sizeof(*server_addr));
    if (sent_size < 0) {
        perror("Failed to send message");
        exit(EXIT_FAILURE);
    }
    printf("Message sent\n");
}