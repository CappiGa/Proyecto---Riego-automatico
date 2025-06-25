#include "SensorHumedad.h"

SensorHumedad::SensorHumedad(TwoWire &i2cPort) : i2c(i2cPort) {}

bool SensorHumedad::iniciar() {
    if (bme.begin(0x76, &i2c)) return true;
    if (bme.begin(0x77, &i2c)) return true;
    Serial.println("❌ No se detectó el sensor BME280 en 0x76 ni 0x77.");
    return false;
}

int SensorHumedad::leerHumedad() {
    return static_cast<int>(bme.readHumidity());
}

float SensorHumedad::leerTemperatura() {
    return bme.readTemperature();
}

float SensorHumedad::leerPresion() {
    return bme.readPressure() / 100.0F;
}
