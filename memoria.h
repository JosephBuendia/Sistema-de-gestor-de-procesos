#ifndef MEMORIA_H
#define MEMORIA_H

// ===== 3. PILA - GESTOR DE MEMORIA =====
// Cada nodo es un bloque de memoria asignado a un proceso

struct NodoBloque {
    int idBloque;
    int idProceso;
    int tamKB;
    NodoBloque* siguiente;
};

// Variables globales (definidas en memoria.cpp)
extern NodoBloque* cimaMemoria;
extern int siguienteBloque;

// Funciones de la pila
void apilar(NodoBloque*& cima, int idBloque, int idProceso, int tamKB);
bool desapilar(NodoBloque*& cima, int& idBloque, int& idProceso, int& tamKB);
int memoriaUsada(NodoBloque* cima);
int buscarBloquesDeProceso(NodoBloque* cima, int idProceso);
int liberarBloquesDeProceso(NodoBloque*& cima, int idProceso);
void mostrarEstadoMemoria(NodoBloque* cima);
void vaciarPila(NodoBloque*& cima);

// Menu del gestor de memoria
void menuMemoria();

#endif
