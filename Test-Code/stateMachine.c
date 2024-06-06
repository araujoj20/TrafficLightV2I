#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include <stdbool.h>
#include <signal.h>

typedef enum{   
    RED_LED         = 17, 
    YELLOW_LED      = 27, 
    GREEN_LED       = 22, 
    PEDESTIAN_BTN   = 23,
    EMERGENCY_BTN   = 24

} LedPin;

typedef enum {
    STATE_GREEN,
    STATE_YELLOW,
    STATE_RED,
    NUM_STATES
} State;

typedef enum{   
    EMERGENCY_TIME  = 10000,  //20000, 
    PEDESTIAN_TIME  = 8000,  //15000, 
    GREEN_TIME      = 5000,  //30000, 
    YELLOW_TIME     = 1000,  //2000
    RED_TIME        = 2000  //5000

} Time;


typedef State (*StateFunction)();


void lightLed(LedPin LedColor);

State greenState();
State yellowState();
State redState();

StateFunction stateFunctions[NUM_STATES] = {
    greenState,
    yellowState,
    redState
};

volatile bool pedestrianBtn = false;
volatile bool emergencyBtn  = false;

unsigned long lastTime = 0;
volatile int waitTime       = GREEN_TIME;
State currentState = STATE_GREEN;
State nextState = STATE_GREEN;

void pedestrianISR(void) {
    pedestrianBtn = true;
    delay(50);
}

void emergencyISR(void) {
    emergencyBtn = true;
    delay(50);
}

void closeProg(int signal) {
    digitalWrite(RED_LED, LOW);
    digitalWrite(YELLOW_LED, LOW);
    digitalWrite(GREEN_LED, LOW);
    
    exit(0);
}

void setup() {
    wiringPiSetupGpio();
    /* LEDS */
    pinMode(GREEN_LED, OUTPUT);
    pinMode(YELLOW_LED, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    /* BUTTONS */
    pinMode(PEDESTIAN_BTN, INPUT);
    wiringPiISR(PEDESTIAN_BTN, INT_EDGE_RISING, &pedestrianISR);
    pinMode(EMERGENCY_BTN, INPUT);
    wiringPiISR(EMERGENCY_BTN, INT_EDGE_RISING, &emergencyISR);
    /* SIGNALS */
    signal(SIGINT, closeProg);
}

State greenState() {
    lightLed(GREEN_LED);

    waitTime = GREEN_TIME;
    nextState = STATE_YELLOW;
    return STATE_YELLOW;
}

State yellowState() {
    lightLed(YELLOW_LED);

    waitTime = YELLOW_TIME;
    nextState = STATE_RED;
    return STATE_RED;
}

State redState() {
    lightLed(RED_LED);

    waitTime = RED_TIME;
    nextState = STATE_GREEN;
    return STATE_GREEN;
}

void lightLed(LedPin LedColor) {
    digitalWrite(RED_LED,    (LedColor == RED_LED)    ? HIGH : LOW);
    digitalWrite(YELLOW_LED, (LedColor == YELLOW_LED) ? HIGH : LOW);
    digitalWrite(GREEN_LED,  (LedColor == GREEN_LED)  ? HIGH : LOW);
}

int main(void) {
    setup();
    unsigned long currentTime = 0;
    while (1) {
        currentTime = millis();

        if (pedestrianBtn) {
            currentState = STATE_RED;
            currentState = stateFunctions[currentState]();
            pedestrianBtn = false;
            lastTime = currentTime;
            waitTime = PEDESTIAN_TIME;
        }

        if (emergencyBtn) {
            currentState = STATE_YELLOW;
            currentState = stateFunctions[currentState]();
            emergencyBtn = false;
            lastTime = currentTime;
            waitTime = EMERGENCY_TIME;
        }
        
        if (currentTime - lastTime >= waitTime) {
            currentState = stateFunctions[currentState]();
            lastTime = currentTime;
        }

        delay(100);
    }
    return 0;
}