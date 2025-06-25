#pragma once
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <time.h>

#define MAX_GOTAS 10

struct Gota {
    int x, y;
    int velocidad;
    int opacidad;
    bool activa;
};

class PantallaEstado {
public:
    PantallaEstado(TwoWire& wire);
    void iniciar();
    void mostrarEstado(int humedad, float temperatura, float presion, bool automatico, bool enRiego, int umbral, struct tm* tiempo, int horaRiego, int minutoRiego);
    void animarRiego(unsigned long tiempoActual);
    void mostrarEmergencia(const char* mensaje);

private:
    TwoWire& i2c;
    Adafruit_SSD1306 display;  // <- Esto está bien si lo inicializas en el .cpp con parámetros
    Gota gotas[MAX_GOTAS];
};
