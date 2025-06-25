#include <Arduino.h>
#include <WiFi.h>
#include <time.h>

#include "WebServerRiego.h"
#include "SensorHumedad.h"
#include "ControlRiego.h"
#include "IndicadoresLED.h"
#include "BotonManual.h"
#include "PantallaEstado.h"

// Pines hardware
#define RELE_PIN       13
#define VERDE_LED      10
#define AZUL_LED       9
#define ROJO_LED       11
#define AMARILLO_LED   12
#define BOTON_MANUAL   7
#define BOTON_PARADA   6

// WiFi
const char* ssid = "vivo V21 5G";
const char* password = "nigga123";

// I2C buses
TwoWire I2CSensor = TwoWire(0);  // SDA=18, SCL=19
TwoWire I2COLED   = TwoWire(1);  // SDA=20, SCL=21

// Instancias
SensorHumedad sensor(I2CSensor);
ControlRiego riego(RELE_PIN);
IndicadoresLED leds(VERDE_LED, AZUL_LED, ROJO_LED, AMARILLO_LED);
BotonManual botonManual(BOTON_MANUAL);
BotonManual botonParada(BOTON_PARADA);
PantallaEstado pantalla(I2COLED);
WebServerRiego web(sensor, riego);

// Estado global
bool riegoDesdeWeb = false;
bool enRiegoManual = false;
bool riegoInterrumpido = false;
bool estadoParado = false;

unsigned long tiempoInicioRiego = 0;
unsigned long tiempoInicioRiegoManual = 0;
bool estadoAmarillo = false;

bool modoAutomatico = true;
int umbralHumedad = 40;
int horaRiego = 8;
int minutoRiego = 0;
unsigned long duracionProgramada = 10000; // por defecto 10s

// Flags de riego programado
int ultimoMinutoChequeado = -1;
bool riegoProgramadoEjecutado = false;

void configurarNTP() {
    configTime(3600, 3600, "pool.ntp.org", "time.nist.gov");
    Serial.println("\n[INFO] Esperando hora NTP...");
    struct tm timeinfo;
    while (!getLocalTime(&timeinfo)) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n[INFO] Hora sincronizada.");
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n✅ Conectado a WiFi");
    Serial.println("IP: " + WiFi.localIP().toString());

    configurarNTP();
web.setLeds(&leds); // Pasa los LEDs al servidor web

    I2CSensor.begin(18, 19);
    I2COLED.begin(20, 21);

    sensor.iniciar();
    pantalla.iniciar();

    web.setEstadoRiegoWeb(&riegoDesdeWeb);
    web.setDuracionRiego(&tiempoInicioRiego);
    web.setModoAutomatico(&modoAutomatico);
    web.setUmbralHumedad(&umbralHumedad);
    web.setHoraRiego(&horaRiego, &minutoRiego);
    web.setDuracionProgramada(&duracionProgramada);
    web.iniciar();

    leds.estadoNormal();  // Encender LED verde de inicio
}

void loop() {
    web.manejarClientes();

    int humedad = sensor.leerHumedad();
    float temperatura = sensor.leerTemperatura();
    float presion = sensor.leerPresion();

    struct tm tiempo;
    bool horaOK = getLocalTime(&tiempo);
    bool estaRegando = riego.riegoActivo();

    if (estadoParado) {
        pantalla.mostrarEmergencia("Presione para reanudar");
        if (botonManual.fuePresionado()) {
            estadoParado = false;
            riegoInterrumpido = false;
            leds.estadoNormal();
            Serial.println("[INFO] Emergencia desactivada");
        }
        delay(100);
        return;
    }

    pantalla.mostrarEstado(humedad, temperatura, presion, modoAutomatico, estaRegando, umbralHumedad,
                           horaOK ? &tiempo : nullptr, horaRiego, minutoRiego);

    // Reset flag al cambiar de minuto
    if (horaOK && tiempo.tm_min != ultimoMinutoChequeado) {
        ultimoMinutoChequeado = tiempo.tm_min;
        riegoProgramadoEjecutado = false;
    }

    // Parada de emergencia
    if (botonParada.fuePresionado()) {
        riego.detenerRiego();
        estadoParado = true;
        riegoInterrumpido = true;
        leds.estadoParado();
        Serial.println("[INFO] Parada de emergencia activada");
    }

    // Riego programado (una vez por minuto)
    if (modoAutomatico && !enRiegoManual && !riegoDesdeWeb && !riegoInterrumpido && horaOK) {
        if (tiempo.tm_hour == horaRiego && tiempo.tm_min == minutoRiego && !estaRegando && !riegoProgramadoEjecutado) {
            riego.activarRiego();
            leds.riegoAutomatico();
            tiempoInicioRiego = millis();
            riegoProgramadoEjecutado = true;
            Serial.println("[INFO] Riego programado iniciado");
        }
    }

    // Riego por humedad baja
    if (modoAutomatico && !riegoDesdeWeb && !enRiegoManual && !riegoInterrumpido) {
        if (humedad < umbralHumedad && !estaRegando) {
            riego.activarRiego();
            leds.riegoAutomatico();
            tiempoInicioRiego = millis();
            Serial.println("[INFO] Riego por humedad baja iniciado");
        } else if (humedad >= umbralHumedad && estaRegando &&
                   millis() - tiempoInicioRiego >= duracionProgramada) {
            riego.detenerRiego();
            leds.estadoNormal();
            Serial.println("[INFO] Riego por humedad baja finalizado");
        }
    }

    // Finalizar riego automático por tiempo
    if (estaRegando && !riegoDesdeWeb && !enRiegoManual &&
        millis() - tiempoInicioRiego >= duracionProgramada) {
        riego.detenerRiego();
        leds.estadoNormal();
        Serial.println("[INFO] Riego automático finalizado");
    }

    // Riego manual
    if (botonManual.fuePresionado() && !riegoInterrumpido) {
        riego.activarRiego();
        leds.riegoManual();
        enRiegoManual = true;
        tiempoInicioRiegoManual = millis();
        Serial.println("[INFO] Riego manual activado");
    }

    if (enRiegoManual && millis() - tiempoInicioRiegoManual >= 5000) {
        riego.detenerRiego();
        leds.estadoNormal();
        enRiegoManual = false;
        Serial.println("[INFO] Riego manual finalizado");
    }

    // LED parpadeo si riego por web
    if (estaRegando && riegoDesdeWeb) {
        leds.riegoAutomatico();  // Amarillo parpadeando
    }

    leds.actualizar();
    delay(100);
}
