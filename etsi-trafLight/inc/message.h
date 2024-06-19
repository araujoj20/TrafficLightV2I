#ifndef MESSAGE_H
#define MESSAGE_H

#include "../asn1-header/SPATEM.h"
#include "../asn1-header/per_encoder.h"
#include "../asn1-header/per_decoder.h"
#include "../inc/main.h"
#include "../inc/fsm.h"
#include <sys/time.h>
#include <time.h>

#define STATION_ID 0x12345678
#define MAX_OF_INTERSECTIONS 4
#define NUM_OF_INTERSECTIONS 4

#define INTER_NAME1 "Inter 1"
#define INTER_NAME2 "Inter 2"
#define INTER_NAME3 "Inter 3"
#define INTER_NAME4 "Inter 4"

#define INTER_ID1 0x1111
#define INTER_ID2 0x2222
#define INTER_ID3 0x3333
#define INTER_ID4 0x4444

#define INTER_STATUS1 IntersectionStatusObject_failureFlash // 2
#define INTER_STATUS2 IntersectionStatusObject_signalPriorityIsActive // 4
#define INTER_STATUS3 IntersectionStatusObject_trafficDependentOperation // 6
#define INTER_STATUS4 IntersectionStatusObject_failureMode // 8

#define M_SET_INTERSECTION(inters, index)   (inters = spatMessage.spat.intersections.list.array[(index)])
#define M_SET_STATE(inters, state)          ((inters)->states.list.array[0]->state_time_speed.list.array[0]->eventState = (state))
#define M_SET_MAX_TIME(inters, MTime)       (*(inters)->states.list.array[0]->state_time_speed.list.array[0]->timing->maxEndTime = (MTime))
#define M_SET_MIN_TIME(inters, mTime)       ((inters)->states.list.array[0]->state_time_speed.list.array[0]->timing->minEndTime  = (mTime))
#define M_SET_START_TIME(inters, sTime)     (*(inters)->states.list.array[0]->state_time_speed.list.array[0]->timing->startTime  = (sTime))


#define M_INCREMENT_COUNT(inters)           ((inters)->revision = ((inters)->revision + 1) % 127)
#define M_SET_CURR_TIME(curTime)            ((curTime) = getCurrTime())

typedef struct {
    IntersectionID_t intersectionIDs[MAX_OF_INTERSECTIONS];
    e_IntersectionStatusObject intersectionSTATUS[MAX_OF_INTERSECTIONS];
    const char* intersectionNames[MAX_OF_INTERSECTIONS];
} IntersectionData;

extern SPATEM_t spatMessage;
extern SPATEM_t* decSpatMessage;
extern IntersectionState_t* intersectionArray;

extern int daysMonth[];

void genHeader(SPATEM_t* spatMessage);
void genIntersection(IntersectionState_t* intersection, const IntersectionData *interData, int int_index);
void genIntersectionStatus(IntersectionState_t* intersection, int intStatus);
void getStateAndTime(MovementEvent_t* event);
MinuteOfTheYear_t getCurrTime(void);
unsigned int getMillisecondsOfHour();
void printMessage(const SPATEM_t* spatM);
void printHeader(const SPATEM_t* spatM);
void printIntersections(const SPATEM_t* spatM);
void freeMemory(SPATEM_t*);
int checkConstraints(SPATEM_t *spat_message, char *out_buffer, IntersectionState_t *intersectionArray);
int messageInit(SPATEM_t *spatMessage, IntersectionState_t **intersectionArray, const IntersectionData *interData, char *out_buffer, MinuteOfTheYear_t *currTimeSpat);
int encodeBuffer(SPATEM_t *spatMessage, uint8_t *out_buffer,  size_t buffer_size, uint16_t *bytes_enc);
int decodeBuffer(SPATEM_t *decoded_spatem, IntersectionState_t *intersectionArray, char *out_buffer, uint16_t bytes_enc);
void convertMinutesToCalendar(int minutesOfYear, int* months, int* days, int* hours, int* minutes);

#endif // MESSAGE_H