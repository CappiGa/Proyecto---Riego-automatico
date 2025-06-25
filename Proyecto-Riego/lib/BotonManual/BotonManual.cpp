#include "BotonManual.h"

BotonManual::BotonManual(uint8_t p) : pin(p) {
    pinMode(pin, INPUT_PULLUP);
}

bool BotonManual::fuePresionado() {
    static bool ultimoEstado = HIGH;
    bool estadoActual = digitalRead(pin);

    if (ultimoEstado == HIGH && estadoActual == LOW) {
        ultimoEstado = estadoActual;
        return true;
    }

    ultimoEstado = estadoActual;
    return false;
}

bool BotonManual::mantenido(unsigned long ms) {
    bool estado = digitalRead(pin) == LOW;

    if (estado) {
        if (tiempoPresionado == 0) tiempoPresionado = millis();
        if (millis() - tiempoPresionado >= ms) return true;
    } else {
        tiempoPresionado = 0;
    }

    return false;
}
