#ifndef PROCESOS_H
#define PROCESOS_H

#include <string>
using namespace std;

// ===== 1. LISTA ENLAZADA SIMPLE - GESTOR DE PROCESOS =====

struct NodoProceso {
    int id;
    string nombre;
    int prioridad;
    int memoriaKB;
    string estado;          // "Nuevo", "En cola", "Ejecutando", "Terminado"
    NodoProceso* siguiente;
};

// Variables globales (definidas en procesos.cpp)
extern NodoProceso* listaProcesos;
extern int siguienteId;

// Funciones de la lista
void insertarProceso(NodoProceso*& inicio, int id, string nombre, int prioridad, int memoriaKB);
NodoProceso* buscarPorID(NodoProceso* inicio, int id);
NodoProceso* buscarPorEstado(NodoProceso* inicio, string estado);
void mostrarProceso(NodoProceso* p);
int buscarPorNombre(NodoProceso* inicio, string nombre);
bool eliminarProceso(NodoProceso*& inicio, int id);
void listarProcesos(NodoProceso* inicio);
int listarPorEstado(NodoProceso* inicio, string estado);
void vaciarLista(NodoProceso*& inicio);

// Menu del gestor de procesos
void menuProcesos();

#endif
