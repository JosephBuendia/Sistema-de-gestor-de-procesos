#ifndef CPU_H
#define CPU_H

#include <string>
using namespace std;

// ===== 2. COLA DE PRIORIDAD - PLANIFICADOR DE CPU =====
// Lista ordenada por prioridad: el frente es el de mayor prioridad

struct NodoCola {
    int idProceso;
    string nombre;
    int prioridad;
    NodoCola* siguiente;
};

// Variable global (definida en cpu.cpp)
extern NodoCola* frenteCPU;

// Funciones de la cola
void encolar(NodoCola*& frente, int id, string nombre, int prioridad);
bool desencolar(NodoCola*& frente, int& id, string& nombre, int& prioridad);
bool estaEnCola(NodoCola* frente, int id);
int posicionEnCola(NodoCola* frente, int id);
bool eliminarDeCola(NodoCola*& frente, int id);
void mostrarCola(NodoCola* frente);
void vaciarCola(NodoCola*& frente);

// Menu del planificador de CPU
void menuCPU();

#endif
