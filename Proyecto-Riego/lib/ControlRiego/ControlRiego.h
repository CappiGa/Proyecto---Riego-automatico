// ControlRiego.h
#ifndef CONTROL_RIEGO_H
#define CONTROL_RIEGO_H

class ControlRiego {
private:
    int pinRele;
    bool riegoEncendido = false;

public:
    ControlRiego(int pin);
    void activarRiego();
    void detenerRiego();
    bool riegoActivo();
};

#endif
