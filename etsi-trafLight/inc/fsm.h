#ifndef FSM_H
#define FSM_H

#define RASPBERRY 0

#include <stdio.h>
#include <stdlib.h>
#if RASPBERRY
    #include <wiringPi.h>
    #include <wiringPiI2C.h>
#endif
#include <stdbool.h>
#include <signal.h>

typedef enum {
    LED_RED         = 17, 
    LED_YELLOW      = 27, 
    LED_GREEN       = 22, 
    BTN_PEDESTRIAN  = 23,
    BTN_EMERGENCY   = 24
} LedPin;

typedef enum {
    STATE_GREEN,       // MovementPhaseState_permissive_Movement_Allowed,
    STATE_YELLOW,      // MovementPhaseState_permissive_clearance
    STATE_RED,         // MovementPhaseState_stop_And_Remain,
    STATE_EMERGENCY,   // MovementPhaseState_stop_Then_Proceed
    STATE_PEDESTRIAN,   
    STATES_NUMBER
} State;

typedef enum {   
    TIME_EMERGENCY  = 10000,
    TIME_PEDESTRIAN = 8000, 
    TIME_GREEN      = 7000,
    TIME_YELLOW     = 2000,
    TIME_RED        = 4000
} Time;

typedef enum {   
    TIME_MIN_EMERGENCY  = 10000,
    TIME_MIN_PEDESTRIAN = 4000, 
    TIME_MIN_GREEN      = 3500,
    TIME_MIN_YELLOW     = 1000,
    TIME_MIN_RED        = 2000
} Time_min;




typedef struct {
    State state;
    Time time;
    Time_min timeMin;
    LedPin pin;
    State (*stateFunction)();
} Operation;

void lightLed(LedPin ledColor);
State greenState();
State yellowState();
State redState();
State emergencyState();
State pedestrianState();

extern Operation operations[STATES_NUMBER];

extern volatile bool pedestrianBtn;
extern volatile bool emergencyBtn;
extern const unsigned int debounceDelay;

void setup();

#if RASPBERRY == 0
    unsigned long millis();
#endif

#endif // FSM_H
