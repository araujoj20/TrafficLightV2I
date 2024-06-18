#include "../inc/fsm.h"
#include <time.h>

volatile bool pedestrianBtn = false;
volatile bool emergencyBtn  = false;
const unsigned int debounceDelay = 50;

Operation operations[STATES_NUMBER] = {
    {STATE_GREEN,       TIME_GREEN,      LED_GREEN,  greenState},
    {STATE_YELLOW,      TIME_YELLOW,     LED_YELLOW, yellowState},
    {STATE_RED,         TIME_RED,        LED_RED,    redState},
    {STATE_EMERGENCY,   TIME_EMERGENCY,  LED_YELLOW, emergencyState},
    {STATE_PEDESTRIAN,  TIME_PEDESTRIAN, LED_RED,    pedestrianState}
};

#if RASPBERRY == 0
    #define CLOCK_MONOTONIC 1
    unsigned long millis() {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
    }

#endif

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
    #if RASPBERRY
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_YELLOW, LOW);
        digitalWrite(LED_GREEN, LOW);
    #endif
    exit(0);
}

void setup(){
    #if RASPBERRY
        wiringPiSetupGpio();
        pinMode(LED_GREEN,  OUTPUT);
        pinMode(LED_YELLOW, OUTPUT);
        pinMode(LED_RED,    OUTPUT);
        pinMode(BTN_PEDESTRIAN, INPUT);
        pinMode(BTN_EMERGENCY,  INPUT);
        wiringPiISR(BTN_PEDESTRIAN, INT_EDGE_RISING, &pedestrianISR);
        wiringPiISR(BTN_EMERGENCY,  INT_EDGE_RISING, &emergencyISR);
        signal(SIGINT, closeProg);
    #endif
}

void lightLed(LedPin ledColor) {
    #if RASPBERRY
        digitalWrite(LED_RED, (ledColor == LED_RED) ? HIGH : LOW);
        digitalWrite(LED_YELLOW, (ledColor == LED_YELLOW) ? HIGH : LOW);
        digitalWrite(LED_GREEN, (ledColor == LED_GREEN) ? HIGH : LOW);
    #endif
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