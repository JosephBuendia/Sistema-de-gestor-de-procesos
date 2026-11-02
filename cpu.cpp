/*
 * MODULO 2 - PLANIFICADOR DE CPU (Cola de prioridad)
 * Responsable: Integrante 2
 */

#include <iostream>
#include <string>
#include "common.h"
#include "cpu.h"
#include "procesos.h"   // usa la lista para leer y cambiar el estado de los procesos
using namespace std;

// Variable global de este modulo
NodoCola* frenteCPU = NULL;          // frente de la cola de prioridad

// Encola respetando la prioridad (si empatan, entra despues: FIFO)
void encolar(NodoCola*& frente, int id, string nombre, int prioridad) {
    NodoCola* nuevo = new NodoCola;
    nuevo->idProceso = id;
    nuevo->nombre = nombre;
    nuevo->prioridad = prioridad;
    nuevo->siguiente = NULL;

    if (frente == NULL || prioridad < frente->prioridad) {
        nuevo->siguiente = frente;          // pasa a ser el primero
        frente = nuevo;
    } else {
        NodoCola* temp = frente;
        while (temp->siguiente != NULL && temp->siguiente->prioridad <= prioridad) {
            temp = temp->siguiente;
        }
        nuevo->siguiente = temp->siguiente;
        temp->siguiente = nuevo;
    }
}

// Desencola el primer proceso (el de mayor prioridad)
bool desencolar(NodoCola*& frente, int& id, string& nombre, int& prioridad) {
    if (frente == NULL) {
        return false;
    }
    NodoCola* temp = frente;
    id = temp->idProceso;
    nombre = temp->nombre;
    prioridad = temp->prioridad;
    frente = frente->siguiente;
    delete temp;
    return true;
}

// Verifica si un proceso esta en la cola
bool estaEnCola(NodoCola* frente, int id) {
    NodoCola* temp = frente;
    while (temp != NULL) {
        if (temp->idProceso == id) {
            return true;
        }
        temp = temp->siguiente;
    }
    return false;
}

// Busca un proceso en la cola. Devuelve su posicion (1 = frente) o 0 si no esta
int posicionEnCola(NodoCola* frente, int id) {
    int posicion = 1;
    NodoCola* temp = frente;
    while (temp != NULL) {
        if (temp->idProceso == id) {
            return posicion;
        }
        temp = temp->siguiente;
        posicion++;
    }
    return 0;
}

// Elimina un proceso especifico de la cola
bool eliminarDeCola(NodoCola*& frente, int id) {
    NodoCola* actual = frente;
    NodoCola* anterior = NULL;

    while (actual != NULL && actual->idProceso != id) {
        anterior = actual;
        actual = actual->siguiente;
    }
    if (actual == NULL) {
        return false;
    }
    if (anterior == NULL) {
        frente = actual->siguiente;
    } else {
        anterior->siguiente = actual->siguiente;
    }
    delete actual;
    return true;
}

// Muestra la cola actual
void mostrarCola(NodoCola* frente) {
    if (frente == NULL) {
        cout << "  (La cola de CPU esta vacia)" << endl;
        return;
    }
    int posicion = 1;
    NodoCola* temp = frente;
    while (temp != NULL) {
        cout << "  " << posicion << ". [Prioridad " << temp->prioridad << "] ID "
             << temp->idProceso << " - " << temp->nombre << endl;
        temp = temp->siguiente;
        posicion++;
    }
}

// Libera todos los nodos de la cola
void vaciarCola(NodoCola*& frente) {
    while (frente != NULL) {
        NodoCola* temp = frente;
        frente = frente->siguiente;
        delete temp;
    }
}

/* ------------------------ PLANIFICADOR DE CPU ------------------------- */

void encolarProceso() {
    int id = leerEntero("  ID del proceso a encolar: ", 1, 999999);
    NodoProceso* nodo = buscarPorID(listaProcesos, id);
    if (nodo == NULL) {
        cout << "  [!] No existe ese ID." << endl;
        return;
    }
    if (nodo->estado == "Terminado") {
        cout << "  [!] El proceso ya termino." << endl;
        return;
    }
    if (nodo->estado == "Ejecutando") {
        cout << "  [!] El proceso se esta ejecutando." << endl;
        return;
    }
    if (estaEnCola(frenteCPU, id)) {
        cout << "  [!] Ya esta en la cola." << endl;
        return;
    }
    encolar(frenteCPU, id, nodo->nombre, nodo->prioridad);
    nodo->estado = "En cola";
    cout << "  [OK] Proceso encolado." << endl;
}

// La CPU ejecuta un proceso a la vez: En cola -> Ejecutando
void ejecutarSiguiente() {
    NodoProceso* enEjecucion = buscarPorEstado(listaProcesos, "Ejecutando");
    if (enEjecucion != NULL) {
        cout << "  [!] La CPU esta ocupada con el ID " << enEjecucion->id
             << ". Termine ese proceso primero." << endl;
        return;
    }
    int id, prioridad;
    string nombre;
    if (!desencolar(frenteCPU, id, nombre, prioridad)) {
        cout << "  [!] La cola esta vacia." << endl;
        return;
    }
    NodoProceso* nodo = buscarPorID(listaProcesos, id);
    if (nodo != NULL) {
        nodo->estado = "Ejecutando";
    }
    cout << "  >>> EJECUTANDO: ID " << id << " - " << nombre
         << " (Prioridad " << prioridad << ")" << endl;
}

// Ejecutando -> Terminado
void terminarProceso() {
    NodoProceso* nodo = buscarPorEstado(listaProcesos, "Ejecutando");
    if (nodo == NULL) {
        cout << "  [!] No hay ningun proceso en ejecucion." << endl;
        return;
    }
    nodo->estado = "Terminado";
    cout << "  >>> Proceso ID " << nodo->id << " - " << nodo->nombre << " terminado." << endl;
}

// Busca un proceso dentro de la cola y dice en que posicion esta
void buscarEnCola() {
    int id = leerEntero("  ID del proceso a buscar en la cola: ", 1, 999999);
    int posicion = posicionEnCola(frenteCPU, id);
    if (posicion == 0) {
        cout << "  [!] Ese proceso no esta en la cola." << endl;
    } else {
        cout << "  [OK] El proceso ID " << id << " esta en la posicion " << posicion << "." << endl;
    }
}

void menuCPU() {
    int op;
    do {
        cout << endl << "--- PLANIFICADOR DE CPU (Cola de prioridad) ---" << endl;
        cout << " 1. Encolar proceso" << endl;
        cout << " 2. Ejecutar siguiente (desencolar)" << endl;
        cout << " 3. Terminar proceso en ejecucion" << endl;
        cout << " 4. Ver cola actual" << endl;
        cout << " 5. Buscar proceso en la cola" << endl;
        cout << " 0. Volver" << endl;
        op = leerEntero("Opcion: ", 0, 5);

        switch (op) {
            case 1: encolarProceso(); break;
            case 2: ejecutarSiguiente(); break;
            case 3: terminarProceso(); break;
            case 4:
                cout << endl << "  Cola actual:" << endl;
                mostrarCola(frenteCPU);
                break;
            case 5: buscarEnCola(); break;
        }
    } while (op != 0);
}
