#include <wiringPi.h>

#define GREEN 22
#define RED 17

int main(void) {

    wiringPiSetupGpio();
    pinMode(GREEN, OUTPUT);
    pinMode(RED, OUTPUT);

    while (1) {
        delay(500);
        digitalWrite(RED, HIGH);
        delay(500);
        digitalWrite(RED, LOW);
    }

    digitalWrite(RED, LOW);

    pinMode(RED, INPUT);

    return 0;
}

