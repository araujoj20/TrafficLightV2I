#ifndef MESSAGE_H
#define MESSAGE_H

#include "../asn1-header/SPATEM.h"
#include "../asn1-header/per_encoder.h"
#include "../asn1-header/per_decoder.h"
#include "../inc/main.h"


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

extern SPATEM_t spatMessage;
extern SPATEM_t* decoded_spatem;
extern IntersectionState_t* intersectionArray;
extern IntersectionID_t intersectionIDs[MAX_OF_INTERSECTIONS];
extern e_IntersectionStatusObject intersectionSTATUS[MAX_OF_INTERSECTIONS];
extern const char* intersectionNames[MAX_OF_INTERSECTIONS];

void genHeader(SPATEM_t* spatMessage);
void genIntersection(IntersectionState_t* intersection, int int_index);
void genIntersectionStatus(IntersectionState_t* intersection, int intStatus);
void getStateAndTime(MovementEvent_t* event);
MinuteOfTheYear_t getCurrTime(void);
void printHeader(SPATEM_t* spatM);
void printIntersections(const SPATEM_t* spatM);
void freeMemory(void);
int checkConstraints(SPATEM_t *spat_message, char *out_buffer, IntersectionState_t *intersectionArray);
int messageInit(SPATEM_t *spatMessage, IntersectionState_t **intersectionArray, char *out_buffer);
int encodeBuffer(SPATEM_t *spatMessage, uint8_t *out_buffer,  size_t buffer_size, uint16_t *bytes_enc);
int decodeBuffer(SPATEM_t *decoded_spatem, IntersectionState_t *intersectionArray, char *out_buffer, uint16_t bytes_enc);

#endif // MESSAGE_H