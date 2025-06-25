#ifndef INDICADORES_LED_H
#define INDICADORES_LED_H

#include <Arduino.h>

class IndicadoresLED {
public:
    IndicadoresLED(int pinVerde, int pinAzul, int pinRojo, int pinAmarillo);

    void estadoNormal();       // LED verde encendido
    void riegoManual();        // LED azul encendido
    void riegoAutomatico();    // LED amarillo parpadeando
    void estadoParado();       // LED rojo encendido
    void actualizar();         // Se llama en loop() para manejar parpadeos

private:
    int verde, azul, rojo, amarillo;

    bool parpadeoAmarillo = false;
    bool estadoAmarillo = false;
    unsigned long ultimoCambio = 0;
    const unsigned long intervaloParpadeo = 500; // ms
};

#endif
