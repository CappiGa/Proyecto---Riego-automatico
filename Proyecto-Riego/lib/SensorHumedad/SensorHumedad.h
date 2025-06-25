// SensorHumedad.h
#ifndef SENSOR_HUMEDAD_H
#define SENSOR_HUMEDAD_H

#include <Adafruit_BME280.h>
#include <Wire.h>

class SensorHumedad {
public:
    SensorHumedad(TwoWire &i2cPort = Wire);
    bool iniciar();
    int leerHumedad(); // Devuelve humedad relativa en %
    float leerTemperatura();
    float leerPresion();

private:
    Adafruit_BME280 bme;
    TwoWire& i2c;
};

#endif