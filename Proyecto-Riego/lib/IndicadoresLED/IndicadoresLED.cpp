#include "IndicadoresLED.h"

IndicadoresLED::IndicadoresLED(int pinVerde, int pinAzul, int pinRojo, int pinAmarillo)
    : verde(pinVerde), azul(pinAzul), rojo(pinRojo), amarillo(pinAmarillo) {
    pinMode(verde, OUTPUT);
    pinMode(azul, OUTPUT);
    pinMode(rojo, OUTPUT);
    pinMode(amarillo, OUTPUT);

    digitalWrite(verde, LOW);
    digitalWrite(azul, LOW);
    digitalWrite(rojo, LOW);
    digitalWrite(amarillo, LOW);
}

void IndicadoresLED::estadoNormal() {
    digitalWrite(verde, HIGH);
    digitalWrite(azul, LOW);
    digitalWrite(rojo, LOW);
    digitalWrite(amarillo, LOW);
    parpadeoAmarillo = false;
}

void IndicadoresLED::riegoManual() {
    digitalWrite(verde, LOW);
    digitalWrite(azul, HIGH);
    digitalWrite(rojo, LOW);
    digitalWrite(amarillo, LOW);
    parpadeoAmarillo = false;
}

void IndicadoresLED::riegoAutomatico() {
    digitalWrite(verde, LOW);
    digitalWrite(azul, LOW);
    digitalWrite(rojo, LOW);
    parpadeoAmarillo = true;
}

void IndicadoresLED::estadoParado() {
    digitalWrite(verde, LOW);
    digitalWrite(azul, LOW);
    digitalWrite(rojo, HIGH);
    digitalWrite(amarillo, LOW);
    parpadeoAmarillo = false;
}

void IndicadoresLED::actualizar() {
    if (parpadeoAmarillo) {
        unsigned long ahora = millis();
        if (ahora - ultimoCambio >= intervaloParpadeo) {
            ultimoCambio = ahora;
            estadoAmarillo = !estadoAmarillo;
            digitalWrite(amarillo, estadoAmarillo);
        }
    }
}
