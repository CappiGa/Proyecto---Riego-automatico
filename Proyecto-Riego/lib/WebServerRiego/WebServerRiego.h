#ifndef WEBSERVER_RIEGO_H
#define WEBSERVER_RIEGO_H

#include <WebServer.h>
#include "SensorHumedad.h"
#include "ControlRiego.h"
#include "IndicadoresLED.h"  // ✅ Añadido para control de LEDs

class WebServerRiego {
public:
    WebServerRiego(SensorHumedad& sensor, ControlRiego& riego);

    void iniciar();
    void manejarClientes();

    void setEstadoRiegoWeb(bool* estado);
    void setDuracionRiego(unsigned long* duracion);
    void setModoAutomatico(bool* modo);
    void setUmbralHumedad(int* umbral);
    void setHoraRiego(int* hora, int* minuto);
    void setDuracionProgramada(unsigned long* duracion);
    void setLeds(IndicadoresLED* leds);  // ✅ Nuevo método

private:
    WebServer server;
    SensorHumedad& sensor;
    ControlRiego& riego;

    bool* estadoRiegoWeb = nullptr;
    unsigned long* duracionRiego = nullptr;
    bool* modoAutomatico = nullptr;
    int* umbralHumedad = nullptr;
    int* horaRiegoPtr = nullptr;
    int* minutoRiegoPtr = nullptr;
    unsigned long* duracionProgramada = nullptr;
    IndicadoresLED* ledsPtr = nullptr;  // ✅ Nuevo puntero a LEDs

    void configurarRutas();
};

#endif
