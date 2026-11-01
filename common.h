#ifndef COMMON_H
#define COMMON_H

#include <string>
using namespace std;

const int MEMORIA_TOTAL = 1024;   // KB disponibles en el sistema

// Pide un entero y repite hasta que este entre minimo y maximo (definida en main.cpp)
int leerEntero(string mensaje, int minimo, int maximo);

#endif
