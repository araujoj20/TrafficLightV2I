#include "../inc/fsm.h"
#include <time.h>

volatile bool pedestrianBtn = 0;
volatile bool emergencyBtn  = 0;
const unsigned int debounceDelay = 50;

Operation operations[STATES_NUMBER] = {
    {STATE_GREEN,       TIME_GREEN,      TIME_MIN_GREEN,       LED_GREEN,  greenState},
    {STATE_YELLOW,      TIME_YELLOW,     TIME_MIN_YELLOW,      LED_YELLOW, yellowState},
    {STATE_RED,         TIME_RED,        TIME_MIN_RED,         LED_RED,    redState},
    {STATE_PEDESTRIAN,  TIME_PEDESTRIAN, TIME_MIN_PEDESTRIAN,  LED_RED,    pedestrianState},
    {STATE_EMERGENCY,   TIME_EMERGENCY,  TIME_MIN_EMERGENCY,   LED_YELLOW, emergencyState}
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
        pedestrianBtn = 1;
        lastPedestrianPress = currentTime;
    }
}

void emergencyISR(void) {
    static unsigned int lastEmergencyPress = 0;
    unsigned int currentTime = millis();
    if (currentTime - lastEmergencyPress > debounceDelay) {
        emergencyBtn = 1;
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

void clearLeds() {
    #if RASPBERRY
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_YELLOW, LOW);
        digitalWrite(LED_GREEN, LOW);
    #endif
}

void lightLed(Pin ledColor) {
    #if RASPBERRY
        digitalWrite(ledColor, HIGH);
    #endif
}

State greenState() {
    printf("green state in\n");
    clearLeds();
    lightLed(LED_GREEN);
    return STATE_YELLOW;
}

State yellowState() {
    clearLeds();
    lightLed(LED_YELLOW);
    return STATE_RED;
}

State redState() {
    clearLeds();
    lightLed(LED_RED);
    return STATE_GREEN;
}

State emergencyState() {
    static unsigned char state = 1;
    
    state ? clearLeds() : lightLed(LED_RED);
    state = !state;

    return STATE_GREEN;
}

State pedestrianState() {
    clearLeds();
    lightLed(LED_RED);
    lightLed(LED_YELLOW);
    return STATE_GREEN;
}