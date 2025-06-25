#include "PantallaEstado.h"
#include <Adafruit_GFX.h>
#include <Arduino.h>
#include <stdlib.h>

PantallaEstado::PantallaEstado(TwoWire& wire)
    : display(128, 64, &wire, -1), i2c(wire) {
    for (int i = 0; i < MAX_GOTAS; ++i) {
        gotas[i].activa = false;
    }
}

void PantallaEstado::iniciar() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("❌ No se pudo iniciar la pantalla OLED"));
        return;
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("Pantalla lista"));
    display.display();
}

void PantallaEstado::mostrarEstado(int humedad, float temperatura, float presion, bool automatico, bool enRiego, int umbral, struct tm* tiempo, int horaRiego, int minutoRiego) {
    if (enRiego) {
        animarRiego(millis());
        return;
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);

    display.printf("H: %d%%  U: %d%%\n", humedad, umbral);
    display.printf("T: %.1fC  P: %.0fhPa\n", temperatura, presion);
    display.print("Modo: ");
    display.println(automatico ? "Auto" : "Manual");

    display.print("Riego: Inactivo\n");

    if (tiempo) {
        display.printf("Hora: %02d:%02d\n", tiempo->tm_hour, tiempo->tm_min);
    } else {
        display.println("⏱ Sin hora NTP");
    }

    display.printf("Prog.: %02d:%02d\n", horaRiego, minutoRiego);

    display.display();
}

void PantallaEstado::animarRiego(unsigned long tiempoActual) {
    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(40, 0);
    display.println("Regando...");

    for (int i = 0; i < MAX_GOTAS; ++i) {
        if (!gotas[i].activa) {
            if (random(0, 100) < 10) {
                gotas[i].x = random(0, 128);
                gotas[i].y = 0;
                gotas[i].velocidad = 1 + random(1, 3);
                gotas[i].opacidad = 255;
                gotas[i].activa = true;
            }
        } else {
            display.fillCircle(gotas[i].x, gotas[i].y, 2, SSD1306_WHITE);
            gotas[i].y += gotas[i].velocidad;
            gotas[i].opacidad -= 10;

            if (gotas[i].y > 64 || gotas[i].opacidad <= 0) {
                gotas[i].activa = false;
            }
        }
    }

    display.display();
}

void PantallaEstado::mostrarEmergencia(const char* mensaje) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("🚨 SISTEMA PARADO");
    display.println(mensaje);
    display.display();
}
