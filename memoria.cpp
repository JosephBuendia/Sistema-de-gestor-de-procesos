/*
 * MODULO 3 - GESTOR DE MEMORIA (Pila)
 * Responsable: Integrante 3
 */

#include <iostream>
#include <string>
#include "common.h"
#include "memoria.h"
#include "procesos.h"   // usa la lista para validar que el proceso exista
using namespace std;

// Variables globales de este modulo
NodoBloque* cimaMemoria = NULL;      // cima de la pila de memoria
int siguienteBloque = 1;             // numero automatico para bloques

// PUSH: apila un bloque en la cima
void apilar(NodoBloque*& cima, int idBloque, int idProceso, int tamKB) {
    NodoBloque* nuevo = new NodoBloque;
    nuevo->idBloque = idBloque;
    nuevo->idProceso = idProceso;
    nuevo->tamKB = tamKB;
    nuevo->siguiente = cima;                // el nuevo apunta a la antigua cima
    cima = nuevo;                           // ahora el nuevo es la cima
}

// POP: desapila el bloque de la cima y devuelve sus datos
bool desapilar(NodoBloque*& cima, int& idBloque, int& idProceso, int& tamKB) {
    if (cima == NULL) {
        return false;
    }
    NodoBloque* temp = cima;
    idBloque = temp->idBloque;
    idProceso = temp->idProceso;
    tamKB = temp->tamKB;
    cima = cima->siguiente;
    delete temp;
    return true;
}

// Suma los KB de todos los bloques (memoria usada)
int memoriaUsada(NodoBloque* cima) {
    int total = 0;
    NodoBloque* temp = cima;
    while (temp != NULL) {
        total = total + temp->tamKB;
        temp = temp->siguiente;
    }
    return total;
}

// Busca y muestra los bloques de un proceso. Devuelve el total de KB
int buscarBloquesDeProceso(NodoBloque* cima, int idProceso) {
    int total = 0;
    NodoBloque* temp = cima;
    while (temp != NULL) {
        if (temp->idProceso == idProceso) {
            cout << "  Bloque #" << temp->idBloque << " -> " << temp->tamKB << " KB" << endl;
            total = total + temp->tamKB;
        }
        temp = temp->siguiente;
    }
    return total;
}

// Libera TODOS los bloques de un proceso usando una pila auxiliar
// (para conservar el orden de los bloques de los demas procesos)
int liberarBloquesDeProceso(NodoBloque*& cima, int idProceso) {
    NodoBloque* pilaAuxiliar = NULL;
    int idB, idP, tam;
    int liberado = 0;

    // Pasamos los bloques de la pila a la auxiliar, descartando los del proceso
    while (desapilar(cima, idB, idP, tam)) {
        if (idP == idProceso) {
            liberado = liberado + tam;
        } else {
            apilar(pilaAuxiliar, idB, idP, tam);
        }
    }
    // Devolvemos los bloques a la pila original (se restaura el orden)
    while (desapilar(pilaAuxiliar, idB, idP, tam)) {
        apilar(cima, idB, idP, tam);
    }
    return liberado;
}

// Muestra el estado de la memoria
void mostrarEstadoMemoria(NodoBloque* cima) {
    int usada = memoriaUsada(cima);
    int libre = MEMORIA_TOTAL - usada;
    int porcentaje = (usada * 100) / MEMORIA_TOTAL;

    cout << "  Total: " << MEMORIA_TOTAL << " KB | Usada: " << usada
         << " KB | Libre: " << libre << " KB (" << porcentaje << "% en uso)" << endl;

    if (cima == NULL) {
        cout << "  (Pila vacia: no hay bloques asignados)" << endl;
        return;
    }
    cout << "  Bloques (CIMA -> FONDO):" << endl;
    NodoBloque* temp = cima;
    while (temp != NULL) {
        cout << "    Bloque #" << temp->idBloque << " | Proceso " << temp->idProceso
             << " | " << temp->tamKB << " KB";
        if (temp == cima) {
            cout << "  <-- CIMA";
        }
        cout << endl;
        temp = temp->siguiente;
    }
}

// Libera todos los nodos de la pila
void vaciarPila(NodoBloque*& cima) {
    while (cima != NULL) {
        NodoBloque* temp = cima;
        cima = cima->siguiente;
        delete temp;
    }
}

/* ------------------------- GESTOR DE MEMORIA -------------------------- */

void asignarMemoria() {
    int id = leerEntero("  ID del proceso: ", 1, 999999);
    NodoProceso* nodo = buscarPorID(listaProcesos, id);
    if (nodo == NULL) {
        cout << "  [!] No existe ese ID." << endl;
        return;
    }
    cout << "  Memoria requerida por el proceso: " << nodo->memoriaKB << " KB" << endl;
    int tam = leerEntero("  KB a asignar (1-1024): ", 1, MEMORIA_TOTAL);
    int libre = MEMORIA_TOTAL - memoriaUsada(cimaMemoria);

    if (tam > libre) {
        cout << "  [!] Memoria insuficiente. Libre: " << libre << " KB." << endl;
    } else {
        apilar(cimaMemoria, siguienteBloque, id, tam);
        siguienteBloque++;
        cout << "  [OK] Bloque asignado (PUSH)." << endl;
    }
}

void liberarMemoria() {
    int idB, idP, tam;
    if (desapilar(cimaMemoria, idB, idP, tam)) {
        cout << "  [OK] Liberado (POP): Bloque #" << idB << " del proceso "
             << idP << " (" << tam << " KB)." << endl;
    } else {
        cout << "  [!] No hay bloques para liberar." << endl;
    }
}

void bloquesDeProceso() {
    int id = leerEntero("  ID del proceso: ", 1, 999999);
    int total = buscarBloquesDeProceso(cimaMemoria, id);
    if (total == 0) {
        cout << "  [!] El proceso no tiene memoria asignada." << endl;
    } else {
        cout << "  Total del proceso: " << total << " KB" << endl;
    }
}

void menuMemoria() {
    int op;
    do {
        cout << endl << "--- GESTOR DE MEMORIA (Pila) ---" << endl;
        cout << " 1. Asignar memoria (push)" << endl;
        cout << " 2. Liberar memoria (pop)" << endl;
        cout << " 3. Estado de la memoria" << endl;
        cout << " 4. Buscar bloques de un proceso" << endl;
        cout << " 0. Volver" << endl;
        op = leerEntero("Opcion: ", 0, 4);

        switch (op) {
            case 1: asignarMemoria(); break;
            case 2: liberarMemoria(); break;
            case 3:
                cout << endl;
                mostrarEstadoMemoria(cimaMemoria);
                break;
            case 4: bloquesDeProceso(); break;
        }
    } while (op != 0);
}
