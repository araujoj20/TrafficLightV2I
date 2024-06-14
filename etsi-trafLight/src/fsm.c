#include "../inc/fsm.h"


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