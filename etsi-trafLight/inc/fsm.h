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
    STATE_GREEN,       // = MovementPhaseState_permissive_Movement_Allowed,
    STATE_YELLOW,
    STATE_RED,         //  = MovementPhaseState_stop_And_Remain,
    STATE_EMERGENCY,
    STATE_PEDESTRIAN,
    STATES_NUMBER
} State;

typedef enum {   
    TIME_EMERGENCY  = 10000,
    TIME_PEDESTRIAN = 8000, 
    TIME_GREEN      = 5000,
    TIME_YELLOW     = 1000,
    TIME_RED        = 2000
} Time;

typedef struct {
    State state;
    Time time;
    LedPin pin;
    State (*nextStateFunction)();
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
