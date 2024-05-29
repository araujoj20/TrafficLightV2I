#include <stdio.h>
#include <sys/types.h>
#include "asn1-header/SPATEM.h"
#include "asn1-header/per_encoder.h"
#include "asn1-header/per_decoder.h"

#include "unistd.h"

/* Prototypes */

void genHeader(SPATEM_t* spatMessage);
void genIntersection(IntersectionState_t* intersection, int int_index);
void genIntersectionStatus(IntersectionState_t* intersection, int intStatus);
void getStateAndTime(MovementEvent_t* event);
MinuteOfTheYear_t getCurrTime(void);

int createSocket();
void configureServerAddress(struct sockaddr_in *server_addr);
void sendMessage(int sockfd, struct sockaddr_in *server_addr, char *buffer, unsigned int bytes_enc);

/* Defines*/

#define STATION_ID 0x12345678          
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
#define MAX_MSG_SIZE 1024       


/* Global Variables*/

SPATEM_t spatMessage;
IntersectionID_t intersectionIDs[NUM_OF_INTERSECTIONS] = {INTERSECTION_ID1, INTERSECTION_ID2, INTERSECTION_ID3, INTERSECTION_ID4};
e_IntersectionStatusObject intersectionSTATUS [NUM_OF_INTERSECTIONS] = {STATUS_1, STATUS_2, STATUS_3, STATUS_4};

/* Functions */

int main(int argc, char **argv)
{
    asn_enc_rval_t ec; // Encoder return value
    asn_dec_rval_t dc; // Decoder return value

    struct sockaddr_in server_addr;
    int sockfd = createSocket();
    configureServerAddress(&server_addr);

    /* HEADER */
    genHeader(&spatMessage);

    /* Intersection State - the only mandatory data frame of the SPAT payload */
    /* Each */
    IntersectionState_t* intersectionArray[NUM_OF_INTERSECTIONS];

    for (int i = 0; i < NUM_OF_INTERSECTIONS; i++) 
    {
        intersectionArray[i] = (IntersectionState_t*)malloc(sizeof(IntersectionState_t));
        if (intersectionArray[i] == NULL)
        {
            printf("Memory allocation failed.\n");
            return -1;
        }
    }

    spatMessage.spat.intersections.list.array = intersectionArray;
    spatMessage.spat.intersections.list.count = NUM_OF_INTERSECTIONS;

    for(int i = 1; i <= NUM_OF_INTERSECTIONS; i++)
    {
        genIntersection(intersectionArray[i], i);
    }

    /* Get the current time */
    MinuteOfTheYear_t currTimeSpat = getCurrTime();

    /* Time Stamp - Current time in MinuteOfTheYear_t format */
    spatMessage.spat.timeStamp = &currTimeSpat;




    uint16_t index = 0;
    int ret;
    uint8_t out_buffer[1024] = {0};
    unsigned int bytes_enc = 0;
    
 





    printf("Checking for constraints Intersection State List...\n");
    ret = asn_check_constraints(&asn_DEF_IntersectionStateList, &spatMessage.spat.intersections.list, out_buffer, (size_t*)&index);

    if(ret)
    {
        printf("Error checking constraints: %s\n", out_buffer);
        return -1;
    }

    printf("Checking for constraints SPATEM...\n");
    ret = asn_check_constraints(&asn_DEF_SPATEM, &spatMessage, out_buffer, (size_t*)&index);

    if(ret)
    {
        printf("Error checking constraints: %s\n", out_buffer);
        return -1;
    }

    memset(out_buffer, 0, sizeof(out_buffer));

    printf("ITS PDU Header: protocol Version: %ld, messageID: %ld, stationID: %ld\n",
            spatMessage.header.protocolVersion, spatMessage.header.messageID,
            spatMessage.header.stationID);

    if(spatMessage.spat.intersections.list.array)
    {
        printf("SPAT payload: timeStamp: %ld, intersection ID: %ld, intersection Name: %*s\n",
            *(spatMessage.spat.timeStamp), spatMessage.spat.intersections.list.array[0]->id.id,
            (int)spatMessage.spat.intersections.list.array[0]->name->size, 
            spatMessage.spat.intersections.list.array[0]->name->buf);
    }

    printf("\nPreparing to encode the message...\n");

    ec = uper_encode_to_buffer(&asn_DEF_SPATEM, NULL, &spatMessage, out_buffer, 1024);

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

    int count = 0;
    while(count++ < 30) {
        
        sendMessage(sockfd, &server_addr, out_buffer, bytes_enc);


        sleep(1);
    }

    close(sockfd);

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

    /* Station ID - identifier of the ITS station that sent the message */
    spatMessage->header.stationID = STATION_ID;

    return;
}

/* Generate one Intersection payload */
void genIntersection(IntersectionState_t* intersection, int index)
{
    /* The use of Regional Extensions is not yet supported by this program*/
    intersection->regional = NULL;

    /* Intersection Reference ID - identifier of the intersection. Unique value in the region or country (255 - 65535) */
    intersection->id.id = intersectionIDs[index-1];

    /* MsgCount - a number that is incremented after each message with the same type (SPAT in this case) and sent by the same sender (0 - 127) */
    intersection->revision = (intersection->revision == 127) ? 0 : (intersection->revision + 1);

    /* Intersection Status Object - contains extra status information about the controller or intersection */
    int intStatus = intersectionSTATUS[index-1]; 
    genIntersectionStatus(intersection, intStatus);
    
    /* Since this is a simple traffic signal station, the program will allow only one movement state and event */
    MovementState_t state;
    MovementState_t* stateArray[1];
    MovementEvent_t event;
    MovementEvent_t* eventArray[1];    

    /* Connection of the state to the list of states */
    intersection->states.list.array = stateArray;
    intersection->states.list.array[0] = &state;
    intersection->states.list.count = 1;
    intersection->states.list.size = 1;    

    /* Connection of the event to the list of events */
    state.state_time_speed.list.array = eventArray;
    state.state_time_speed.list.array[0] = &event;
    state.state_time_speed.list.count = 1;
    state.state_time_speed.list.size = 1;

    /* Signal Group ID - identifier that represent the current movement, phase and topology (0 - 255) */
    /* It's related to the MAP message, so its value = 0, which means the ID is not available or not known */
    state.signalGroup = 0;

    /* Get the current phase signal and the respective timings */
    getStateAndTime(&event);
}

/* Generate the appropriate BIT STRING to the specified intersection status */
void genIntersectionStatus(IntersectionState_t* intersection, int intStatus)
{
    /* Calculates the byte index of the buffer where the bit should be written */
    int bufferIndex = intStatus / 8;
    /* Calculates the bit position in the byte specified by bufferIndex */
    int bufferOffset = intStatus % 8;

    /* Enables the correspondent bit in the buffer */
    intersection->status.buf[bufferIndex] = (1 << bufferOffset);
    /* Size - size of the buffer (bytes) */
    intersection->status.size = bufferIndex + 1;
    /* Bits Unused - number of bits not used (0) in the last byte/octet */
    intersection->status.bits_unused = 7 - bufferOffset;   
}



/* Get the timing values for the signal */
void getStateAndTime(MovementEvent_t* event)
{
    // TODO implementar metodo para ler estado do semaforo na maquina de estados e os respetivos timings

    /* Movement Phase and State - represents the current state of the traffic signal */
    event->eventState = MovementPhaseState_protected_clearance;    

    /* Start Time - time when the future phase signal will start */
    event->timing->startTime = 500;

    /* Min End Time - shortest end time for the current phase signal */
    event->timing->minEndTime = 30;

    /* Max End Time - longest end time for the current phase signal */
    event->timing->maxEndTime = 70;
}

/* Get the current time value in MinuteOfTheYear_t format */
MinuteOfTheYear_t getCurrTime(void)
{
    // TODO implementar metodo para tempo atual
    return 20000;
}

/* -------------------------------------- COMMS --------------------------------------*/

int createSocket() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

void configureServerAddress(struct sockaddr_in *server_addr) {
    memset(server_addr, 0, sizeof(*server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(PORT);
    if (inet_aton(SERVER_IP, &server_addr->sin_addr) == 0) {
        perror("Invalid IP address");
        exit(EXIT_FAILURE);
    }
}

void sendMessage(int sockfd, struct sockaddr_in *server_addr, char *buffer, unsigned int bytes_enc) {
    if (sendto(sockfd, buffer, bytes_enc, 0, (struct sockaddr *)server_addr, sizeof(*server_addr)) == -1) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }
}


