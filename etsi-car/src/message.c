#include "../inc/message.h"

SPATEM_t spatMessage;
SPATEM_t* decoded_spatem = &spatMessage;
IntersectionState_t* intersectionArray;
IntersectionID_t intersectionIDs[MAX_OF_INTERSECTIONS] = {INTERSECTION_ID1, INTERSECTION_ID2, INTERSECTION_ID3, INTERSECTION_ID4};
e_IntersectionStatusObject intersectionSTATUS[MAX_OF_INTERSECTIONS] = {STATUS_1, STATUS_2, STATUS_3, STATUS_4};
const char* intersectionNames[MAX_OF_INTERSECTIONS] = {"First INT", "Second INT", "Third INT", "Fourth INT"};

/* -------------------------- Static Functions -----------------------------*/
static void printIntersectionInfo(const IntersectionState_t* intersection);

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
        DEBUG_PRINT("Memory allocation failed - Intersection Name\n");
        exit(-1);
    }
    size_t name_len = strlen(intersectionNames[i]);
    intersection->name->buf = (uint8_t*) malloc(name_len + 1);
    if (intersection->name->buf == NULL) {
        DEBUG_PRINT("Memory allocation failed - Intersection Name Buffer\n");
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
        DEBUG_PRINT("Memory allocation failed - MovementState_t*\n");
        exit(-1);
    }
    intersection->states.list.array = (MovementState_t**) malloc(1 * sizeof(MovementState_t*));
    if (intersection->states.list.array == NULL) {
        DEBUG_PRINT("Memory allocation failed - intersection->states.list.array\n");
        free(movementArray);
        exit(-1);
    }
    intersection->states.list.array[0] = &movementArray[0];
    intersection->states.list.count = 1;
    intersection->states.list.size = 1 * sizeof(MovementState_t*);

    /* Initialize the MovementState_t array */
    movementArray[0].movementName = (DescriptiveName_t*) malloc(sizeof(DescriptiveName_t));
    if (movementArray[0].movementName == NULL) {
        DEBUG_PRINT("Memory allocation failed - Movement Name\n");
        exit(-1);
    }
    const char* movementName = "Sample Movement";
    size_t movementName_len = strlen(movementName);
    movementArray[0].movementName->buf = (uint8_t*) malloc(movementName_len + 1);
    if (movementArray[0].movementName->buf == NULL) {
        DEBUG_PRINT("Memory allocation failed - Movement Name Buffer\n");
        exit(-1);
    }
    memcpy(movementArray[0].movementName->buf, movementName, movementName_len);
    movementArray[0].movementName->buf[movementName_len] = '\0';
    movementArray[0].movementName->size = movementName_len;

    /* MovementEvent */
    MovementEvent_t* movementEventArray = (MovementEvent_t*) malloc(1 * sizeof(MovementEvent_t));
    if (movementEventArray == NULL) {
        DEBUG_PRINT("Memory allocation failed - MovementEvent_t*\n");
        exit(-1);
    }
    movementArray[0].state_time_speed.list.array = (MovementEvent_t**) malloc(1 * sizeof(MovementEvent_t*));
    if (movementArray[0].state_time_speed.list.array == NULL) {
        DEBUG_PRINT("Memory allocation failed - movementArray[0].state_time_speed.list.array\n");
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
        DEBUG_PRINT("Memory allocation failed - intersection->status.buf\n");
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
        DEBUG_PRINT("Memory allocation failed - TimeChangeDetails_t\n");
        exit(-1);
    }

    /* Set the startTime */
    event->timing->startTime = (TimeMark_t*) malloc(sizeof(TimeMark_t));
    if (event->timing->startTime == NULL) {
        DEBUG_PRINT("Memory allocation failed - startTime\n");
        exit(-1);
    }
    *(event->timing->startTime) = 10;

    /* Set the minEndTime */
    event->timing->minEndTime = 20;

    /* Set the maxEndTime */
    event->timing->maxEndTime = (TimeMark_t*) malloc(sizeof(TimeMark_t));
    if (event->timing->maxEndTime == NULL) {
        DEBUG_PRINT("Memory allocation failed - maxEndTime\n");
        exit(-1);
    }
    *(event->timing->maxEndTime) = 30;
}

/* Get the current time */
MinuteOfTheYear_t getCurrTime(void)
{
    struct timeval tmVal;
    gettimeofday(&tmVal, NULL);

    struct tm *tmInfo;
    tmInfo = localtime(&tmVal.tv_sec);

    MinuteOfTheYear_t minutesYear = (tmInfo->tm_yday * 24 * 60) + (tmInfo->tm_hour * 60) + tmInfo->tm_min;
    
    int secondsHour = tmInfo->tm_min * 60 + tmInfo->tm_sec;
    int tenthsSecond = (tmVal.tv_usec / 100000) % 10;

    //DEBUG_PRINT("Seconds of the Hour : %d\n", secondsHour);
    //DEBUG_PRINT("Tenths of a second in relation to the Hour: %d\n", tenthsSecond + secondsHour * 10);

    return minutesYear;
}

/* Print Message Header */
void printHeader(SPATEM_t* spatM){

    DEBUG_PRINT("-----------------------------------------------------------------------------------\n"
            "PDU Header:\nProtocol Version: %ld     Message ID: %ld     Station ID: 0x%lx     Time: %ld\n"
            "-----------------------------------------------------------------------------------\n",
            spatM->header.protocolVersion, spatM->header.messageID,
            spatM->header.stationID, *(spatM->spat.timeStamp));
}

void printIntersections(const SPATEM_t* spatM)
{
    if(spatM->spat.intersections.list.array)
    {
        for (int i = 0; i < NUM_OF_INTERSECTIONS; i++){
            printIntersectionInfo(spatM->spat.intersections.list.array[i]);
        }
    }
}

/* Print intersection information */
static void printIntersectionInfo(const IntersectionState_t* intersection)
{
    if (!intersection) 
    {   
        DEBUG_PRINT("Error! Intersection is NULL\n");
        return;
    }

    MovementState_t* state = intersection->states.list.array[0];
    MovementEvent_t* event = state->state_time_speed.list.array[0];

    DEBUG_PRINT(" Intersection ID     : 0x%-13lx|\t", intersection->id.id);
    DEBUG_PRINT(" Traffic Light State : %ld\n", event->eventState);
    DEBUG_PRINT(" Intersection Name   : %-15s|\t", intersection->name->buf);
    DEBUG_PRINT(" Start Time          : %ld\n", *(event->timing->startTime));
    DEBUG_PRINT(" Intersection Status : 0x%02x           |\t", intersection->status.buf[0]);
    DEBUG_PRINT(" Min End Time        : %ld\n", event->timing->minEndTime);
    DEBUG_PRINT(" Message Count       : %-15ld|\t", intersection->revision);
    DEBUG_PRINT(" Max End Time        : %ld\n", *(event->timing->maxEndTime));
    DEBUG_PRINT("-----------------------------------------------------------------------------------\n");
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

int checkConstraints(SPATEM_t *spat_message, char *out_buffer, IntersectionState_t *intersectionArray){
    int ret;
    size_t index = 0;
    
    ret = asn_check_constraints(&asn_DEF_IntersectionStateList, &spat_message->spat.intersections.list, out_buffer, &index);
    if (ret) {
        DEBUG_PRINT("Error checking constraints - Intersection State List: %s\n", out_buffer);
        free(intersectionArray);
        free(spat_message->spat.intersections.list.array);
        return -1;
    }

    ret = asn_check_constraints(&asn_DEF_SPATEM, spat_message, out_buffer, &index);
    if (ret) {
        DEBUG_PRINT("Error checking constraints - SPATEM: %s\n", out_buffer);
        free(intersectionArray);
        free(spat_message->spat.intersections.list.array);
        return -1;
    }
    
    return 0;
}

int messageInit(SPATEM_t *spatMessage, IntersectionState_t **intersectionArray, char *out_buffer, MinuteOfTheYear_t *currTimeSpat){
    
    // HEADER
    genHeader(spatMessage);

    // Allocate memory for the intersection array
    *intersectionArray = (IntersectionState_t*) malloc(NUM_OF_INTERSECTIONS * sizeof(IntersectionState_t));
    if (*intersectionArray == NULL) {
        DEBUG_PRINT("Memory allocation failed - IntersectionState_t*\n");
        return -1;
    }

    // Allocate memory for the array of the intersections list
    spatMessage->spat.intersections.list.array = (IntersectionState_t**) malloc(NUM_OF_INTERSECTIONS * sizeof(IntersectionState_t*));
    if (spatMessage->spat.intersections.list.array == NULL) {
        DEBUG_PRINT("Memory allocation failed - spatMessage.spat.intersections.list.array\n");
        free(*intersectionArray);
        return -1;
    }

    // Initialize the array pointers
    for (int i = 0; i < NUM_OF_INTERSECTIONS; i++) {
        spatMessage->spat.intersections.list.array[i] = &(*intersectionArray)[i];
    }

    spatMessage->spat.intersections.list.count = NUM_OF_INTERSECTIONS;
    spatMessage->spat.intersections.list.size = NUM_OF_INTERSECTIONS * sizeof(IntersectionState_t*);

    // Generate each intersection
    for (int i = 0; i < NUM_OF_INTERSECTIONS; i++) {
        genIntersection(&(*intersectionArray)[i], i);
    }

    // Check constraints
    if (checkConstraints(spatMessage, out_buffer, *intersectionArray) != 0)
        return -1;

    // Clear out_buffer
    memset(out_buffer, 0, sizeof(out_buffer));

    spatMessage->spat.timeStamp = &currTimeSpat;

    return 0;
}

int encodeBuffer(SPATEM_t *spatMessage, uint8_t *out_buffer,  size_t buffer_size, uint16_t *bytes_enc){
    DEBUG_PRINT("\nPreparing to encode the message...\n");
    asn_enc_rval_t ec = uper_encode_to_buffer(&asn_DEF_SPATEM, NULL, spatMessage, out_buffer, buffer_size);
    *bytes_enc = (ec.encoded + 7) / 8;

    if (ec.encoded > 0) {
        DEBUG_PRINT("Bits encoded: %ld\n", ec.encoded);
        for (uint16_t index = 0; index < *bytes_enc; index++){
            DEBUG_PRINT("0x%02x ", out_buffer[index]);
            index++;
        }
        DEBUG_PRINT("\n");
    } else {
        DEBUG_PRINT("Error while encoding: %s\n", ec.failed_type ? ec.failed_type->name : "unknown");
        return -1;
    }
    return 0;
}

int decodeBuffer(SPATEM_t *decoded_spatem, IntersectionState_t *intersectionArray, char *out_buffer, uint16_t bytes_enc){
    
    DEBUG_PRINT("\nPreparing to decode the message...\n");

    asn_dec_rval_t dc = uper_decode_complete(NULL, &asn_DEF_SPATEM, (void **)&decoded_spatem, out_buffer, bytes_enc);
    if (dc.code != RC_OK) {
        DEBUG_PRINT("Error decoding buffer. Bits decoded: %ld \n", dc.consumed);
        free(intersectionArray);
        free(decoded_spatem->spat.intersections.list.array);
        return -1;
    }

    return 0;
}
