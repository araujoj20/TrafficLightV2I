#ifndef FSM_H
#define FSM_H

#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
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
    STATE_GREEN,
    STATE_YELLOW,
    STATE_RED,
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

Operation operations[STATES_NUMBER] = {
    {STATE_GREEN,       TIME_GREEN,      LED_GREEN,  greenState},
    {STATE_YELLOW,      TIME_YELLOW,     LED_YELLOW, yellowState},
    {STATE_RED,         TIME_RED,        LED_RED,    redState},
    {STATE_EMERGENCY,   TIME_EMERGENCY,  LED_YELLOW, emergencyState},
    {STATE_PEDESTRIAN,  TIME_PEDESTRIAN, LED_RED,    pedestrianState}
};

volatile bool pedestrianBtn = false;
volatile bool emergencyBtn  = false;
const unsigned int debounceDelay = 50;


#endif // FSM_H
