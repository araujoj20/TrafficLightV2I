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

void pedestrianISR(void) {
    static unsigned int lastPedestrianPress = 0;
    unsigned int currentTime = millis();
    if (currentTime - lastPedestrianPress > debounceDelay) {
        pedestrianBtn = true;
        lastPedestrianPress = currentTime;
    }
}

void emergencyISR(void) {
    static unsigned int lastEmergencyPress = 0;
    unsigned int currentTime = millis();
    if (currentTime - lastEmergencyPress > debounceDelay) {
        emergencyBtn = true;
        lastEmergencyPress = currentTime;
    }
}

void closeProg(int signal) {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_GREEN, LOW);
    exit(0);
}

void setup() {
    wiringPiSetupGpio();
    pinMode(LED_GREEN,  OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_RED,    OUTPUT);
    pinMode(BTN_PEDESTRIAN, INPUT);
    pinMode(BTN_EMERGENCY,  INPUT);
    wiringPiISR(BTN_PEDESTRIAN, INT_EDGE_RISING, &pedestrianISR);
    wiringPiISR(BTN_EMERGENCY,  INT_EDGE_RISING, &emergencyISR);
    signal(SIGINT, closeProg);
}

void lightLed(LedPin ledColor) {
    digitalWrite(LED_RED, (ledColor == LED_RED) ? HIGH : LOW);
    digitalWrite(LED_YELLOW, (ledColor == LED_YELLOW) ? HIGH : LOW);
    digitalWrite(LED_GREEN, (ledColor == LED_GREEN) ? HIGH : LOW);
}

State greenState() {
    lightLed(LED_GREEN);
    return STATE_YELLOW;
}

State yellowState() {
    lightLed(LED_YELLOW);
    return STATE_RED;
}

State redState() {
    lightLed(LED_RED);
    return STATE_GREEN;
}

State emergencyState() {
    lightLed(LED_YELLOW);
    return STATE_RED;
}

State pedestrianState() {
    lightLed(LED_RED);
    return STATE_GREEN;
}

int main(void) {
    setup();
    unsigned char f_priority = 0;
    unsigned int currentTime = 0;
    unsigned int lastTime = 0;
    unsigned int waitTime = 0;
    State currentState  = STATE_GREEN;
    State nextState     = STATE_GREEN;

    while (1) {
        currentTime = millis();

        if (pedestrianBtn) {
            currentState = STATE_PEDESTRIAN;
            pedestrianBtn = false;
            f_priority = 1;
        }

        if (emergencyBtn) {
            currentState = STATE_EMERGENCY;
            emergencyBtn = false;
            f_priority = 1;
        }

        if ((currentTime - lastTime >= waitTime) || f_priority) {
            nextState = operations[currentState].nextStateFunction();
            waitTime  = operations[currentState].time;
            lastTime  = currentTime;
            currentState = nextState;
            f_priority = 0;
        }

        delay(100);
    }
    return 0;
}
