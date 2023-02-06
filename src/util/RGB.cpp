#include "RGB.h"

#define RED     26
#define GREEN   25
#define BLUE    27

void RGB::begin() {
    WiFiDrv::pinMode(RED, OUTPUT);  //GREEN
    WiFiDrv::pinMode(GREEN, OUTPUT);  //RED
    WiFiDrv::pinMode(BLUE, OUTPUT);  //BLUE
}
void RGB::clear() {
    WiFiDrv::digitalWrite(RED, LOW);
    WiFiDrv::digitalWrite(GREEN, LOW);
    WiFiDrv::digitalWrite(BLUE, LOW);
}

void RGB::redOn() {
    WiFiDrv::digitalWrite(RED, HIGH);
}
void RGB::greenOn() {
    WiFiDrv::digitalWrite(GREEN, HIGH);
}
void RGB::blueOn() {
    WiFiDrv::digitalWrite(BLUE, HIGH);
}

void RGB::redOff() {
    WiFiDrv::digitalWrite(RED, LOW);
}
void RGB::greenOff() {
    WiFiDrv::digitalWrite(GREEN, LOW);
}
void RGB::blueOff() {
    WiFiDrv::digitalWrite(BLUE, LOW);
}
