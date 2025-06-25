// ControlRiego.cpp
#include "ControlRiego.h"
#include <Arduino.h>

ControlRiego::ControlRiego(int pin) {
    pinRele = pin;
    pinMode(pinRele, OUTPUT_OPEN_DRAIN);
    digitalWrite(pinRele, LOW);
}

void ControlRiego::activarRiego() {
    digitalWrite(pinRele, HIGH);
    riegoEncendido = true;
}

void ControlRiego::detenerRiego() {
    digitalWrite(pinRele, LOW);
    riegoEncendido = false;
}

bool ControlRiego::riegoActivo() {
    return riegoEncendido;
}
