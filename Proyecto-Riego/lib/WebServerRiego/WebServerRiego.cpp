#include "WebServerRiego.h"
#include <WiFi.h>
#include <Arduino.h>

WebServerRiego::WebServerRiego(SensorHumedad& s, ControlRiego& r)
    : server(80), sensor(s), riego(r) {}

void WebServerRiego::setEstadoRiegoWeb(bool* estado) {
    estadoRiegoWeb = estado;
}

void WebServerRiego::setDuracionRiego(unsigned long* duracion) {
    duracionRiego = duracion;
}

void WebServerRiego::setModoAutomatico(bool* modo) {
    modoAutomatico = modo;
}

void WebServerRiego::setUmbralHumedad(int* umbral) {
    umbralHumedad = umbral;
}

void WebServerRiego::setHoraRiego(int* h, int* m) {
    horaRiegoPtr = h;
    minutoRiegoPtr = m;
}

void WebServerRiego::setDuracionProgramada(unsigned long* duracion) {
    duracionProgramada = duracion;
}

void WebServerRiego::setLeds(IndicadoresLED* l) {
    ledsPtr = l;
}

void WebServerRiego::iniciar() {
    configurarRutas();
    server.begin();
    Serial.println("[INFO] Servidor web iniciado.");
}

void WebServerRiego::manejarClientes() {
    server.handleClient();
}

void WebServerRiego::configurarRutas() {
    server.on("/", [this]() {
        int humedad = sensor.leerHumedad();
        bool enRiego = riego.riegoActivo();
        String modo = (modoAutomatico && *modoAutomatico) ? "Automático" : "Manual";
        String horaRiegoStr = (horaRiegoPtr && minutoRiegoPtr) ? String(*horaRiegoPtr) + ":" + String(*minutoRiegoPtr) : "N/A";
        String duracionStr = duracionProgramada ? String(*duracionProgramada / 1000) + "s" : "N/A";

        String html = R"rawliteral(
            <!DOCTYPE html>
            <html lang='es'>
            <head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>
            <title>🌿 Sistema de Riego</title>
            <style>
                body {
                    background-image: url('https://images.unsplash.com/photo-1501004318641-b39e6451bec6?auto=format&fit=crop&w=1950&q=80');
                    background-size: cover;
                    color: white;
                    font-family: sans-serif;
                }
                .contenedor {
                    background-color: rgba(0, 0, 0, 0.6);
                    padding: 20px;
                    border-radius: 10px;
                    max-width: 500px;
                    margin: auto;
                }
                input, button {
                    margin-top: 10px;
                }
            </style>
            </head>
            <body>
            <div class='contenedor'>
            <h2>🌿 Sistema de Riego</h2>
            <p><strong>Humedad:</strong> )rawliteral" + String(humedad) + R"rawliteral(%</p>
            <p><strong>Modo actual:</strong> )rawliteral" + modo + R"rawliteral(</p>
            <p><strong>Estado:</strong> )rawliteral" + (enRiego ? "Regando 💧" : "Inactivo ☀️") + R"rawliteral(</p>
            <p><strong>Umbral:</strong> )rawliteral" + (umbralHumedad ? String(*umbralHumedad) + "%" : "N/A") + R"rawliteral(</p>
            <p><strong>Hora riego diario:</strong> )rawliteral" + horaRiegoStr + R"rawliteral(</p>
            <p><strong>Duración:</strong> )rawliteral" + duracionStr + R"rawliteral(</p>

            <form action='/modo' method='get'>
                <button name='set' value='auto'>🌱 Modo Automático</button>
                <button name='set' value='manual'>🤖 Modo Manual</button>
            </form>

            <form action='/riego' method='get'>
                <button>💧 Activar / Detener Riego</button>
            </form>

            <form action='/umbral' method='get'>
                <input type='number' name='valor' min='0' max='100' value=')rawliteral" + (umbralHumedad ? String(*umbralHumedad) : "") + R"rawliteral('>
                <button>Guardar Umbral</button>
            </form>

            <form action='/hora' method='get'>
                <input type='number' name='hora' min='0' max='23' value=')rawliteral" + String(horaRiegoPtr ? *horaRiegoPtr : 0) + R"rawliteral('> :
                <input type='number' name='minuto' min='0' max='59' value=')rawliteral" + String(minutoRiegoPtr ? *minutoRiegoPtr : 0) + R"rawliteral('>
                <button>Guardar Hora</button>
            </form>

            <form action='/duracion' method='get'>
                <input type='number' name='segundos' min='1' max='600' value=')rawliteral" + (duracionProgramada ? String(*duracionProgramada / 1000) : "") + R"rawliteral('>
                <button>Guardar Duración</button>
            </form>
            </div>
            </body></html>
        )rawliteral";

        server.send(200, "text/html", html);
    });

    server.on("/modo", HTTP_GET, [this]() {
        if (modoAutomatico) {
            String setModo = server.arg("set");
            *modoAutomatico = (setModo == "auto");
            Serial.println(*modoAutomatico ? "[INFO] Modo AUTO activado" : "[INFO] Modo MANUAL activado");
        }
        server.sendHeader("Location", "/", true);
        server.send(302, "text/plain", "Redireccionando...");
    });

    server.on("/riego", HTTP_GET, [this]() {
        if (riego.riegoActivo()) {
            riego.detenerRiego();
            if (estadoRiegoWeb) *estadoRiegoWeb = false;
            if (ledsPtr) ledsPtr->estadoNormal();  // 🔄 Reset LEDs
            Serial.println("[INFO] Riego detenido desde la web");
        } else {
            riego.activarRiego();
            if (estadoRiegoWeb) *estadoRiegoWeb = true;
            if (ledsPtr) ledsPtr->riegoAutomatico();  // 🔄 Encender amarillo
            Serial.println("[INFO] Riego activado desde la web");
        }
        server.sendHeader("Location", "/", true);
        server.send(302, "text/plain", "Redireccionando...");
    });

    server.on("/umbral", HTTP_GET, [this]() {
        if (umbralHumedad) {
            int nuevo = server.arg("valor").toInt();
            if (nuevo >= 0 && nuevo <= 100) {
                *umbralHumedad = nuevo;
                Serial.printf("[INFO] Umbral cambiado a: %d%%\n", nuevo);
            }
        }
        server.sendHeader("Location", "/", true);
        server.send(302, "text/plain", "Redireccionando...");
    });

    server.on("/hora", HTTP_GET, [this]() {
        if (horaRiegoPtr && minutoRiegoPtr) {
            int h = server.arg("hora").toInt();
            int m = server.arg("minuto").toInt();
            if (h >= 0 && h < 24 && m >= 0 && m < 60) {
                *horaRiegoPtr = h;
                *minutoRiegoPtr = m;
                Serial.printf("[INFO] Hora programada: %02d:%02d\n", h, m);
            }
        }
        server.sendHeader("Location", "/", true);
        server.send(302, "text/plain", "Redireccionando...");
    });

    server.on("/duracion", HTTP_GET, [this]() {
        if (duracionProgramada) {
            int s = server.arg("segundos").toInt();
            if (s > 0 && s <= 600) {
                *duracionProgramada = s * 1000;
                Serial.printf("[INFO] Duración de riego: %d segundos\n", s);
            }
        }
        server.sendHeader("Location", "/", true);
        server.send(302, "text/plain", "Redireccionando...");
    });
}
