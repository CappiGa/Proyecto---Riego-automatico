#pragma once
#include <Arduino.h>

class BotonManual {
private:
    uint8_t pin;
    unsigned long tiempoPresionado = 0;

public:
    BotonManual(uint8_t p);
    bool fuePresionado();
    bool mantenido(unsigned long ms);
};
