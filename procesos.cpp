/*
 * MODULO 1 - GESTOR DE PROCESOS (Lista enlazada simple)
 * Responsable: Integrante 1
 */

#include <iostream>
#include <string>
#include "common.h"
#include "procesos.h"
#include "cpu.h"        // quitarProceso y modificarPrioridad usan la cola
#include "memoria.h"    // quitarProceso libera la memoria del proceso
using namespace std;

// Variables globales de este modulo
NodoProceso* listaProcesos = NULL;   // inicio de la lista
int siguienteId = 1;                 // ID automatico para procesos

// Inserta un proceso al final de la lista
void insertarProceso(NodoProceso*& inicio, int id, string nombre, int prioridad, int memoriaKB) {
    NodoProceso* nuevo = new NodoProceso;   // creamos el nodo en memoria dinamica
    nuevo->id = id;
    nuevo->nombre = nombre;
    nuevo->prioridad = prioridad;
    nuevo->memoriaKB = memoriaKB;
    nuevo->estado = "Nuevo";
    nuevo->siguiente = NULL;

    if (inicio == NULL) {                   // lista vacia: el nuevo es el inicio
        inicio = nuevo;
    } else {
        NodoProceso* temp = inicio;
        while (temp->siguiente != NULL) {   // buscamos el ultimo nodo
            temp = temp->siguiente;
        }
        temp->siguiente = nuevo;            // enlazamos el ultimo con el nuevo
    }
}

// Busca un proceso por ID. Devuelve el nodo o NULL si no existe
NodoProceso* buscarPorID(NodoProceso* inicio, int id) {
    NodoProceso* temp = inicio;
    while (temp != NULL) {
        if (temp->id == id) {
            return temp;
        }
        temp = temp->siguiente;
    }
    return NULL;
}

// Devuelve el primer proceso que este en un estado dado (o NULL si no hay)
NodoProceso* buscarPorEstado(NodoProceso* inicio, string estado) {
    NodoProceso* temp = inicio;
    while (temp != NULL) {
        if (temp->estado == estado) {
            return temp;
        }
        temp = temp->siguiente;
    }
    return NULL;
}

// Muestra los datos de un proceso
void mostrarProceso(NodoProceso* p) {
    cout << "  ID: " << p->id << " | " << p->nombre
         << " | Prioridad: " << p->prioridad
         << " | Memoria: " << p->memoriaKB << " KB"
         << " | Estado: " << p->estado << endl;
}

// Busca por nombre (igual al escrito) y muestra las coincidencias
int buscarPorNombre(NodoProceso* inicio, string nombre) {
    int encontrados = 0;
    NodoProceso* temp = inicio;
    while (temp != NULL) {
        if (temp->nombre == nombre) {
            mostrarProceso(temp);
            encontrados++;
        }
        temp = temp->siguiente;
    }
    return encontrados;
}

// Elimina un proceso por ID. Devuelve true si lo elimino
bool eliminarProceso(NodoProceso*& inicio, int id) {
    NodoProceso* actual = inicio;
    NodoProceso* anterior = NULL;

    while (actual != NULL && actual->id != id) {
        anterior = actual;
        actual = actual->siguiente;
    }
    if (actual == NULL) {                   // no existe
        return false;
    }
    if (anterior == NULL) {                 // era el primer nodo
        inicio = actual->siguiente;
    } else {
        anterior->siguiente = actual->siguiente;
    }
    delete actual;                          // liberamos la memoria del nodo
    return true;
}

// Lista todos los procesos
void listarProcesos(NodoProceso* inicio) {
    if (inicio == NULL) {
        cout << "  (No hay procesos registrados)" << endl;
        return;
    }
    NodoProceso* temp = inicio;
    while (temp != NULL) {
        mostrarProceso(temp);
        temp = temp->siguiente;
    }
}

// Lista solo los procesos que estan en un estado dado. Devuelve cuantos hay
int listarPorEstado(NodoProceso* inicio, string estado) {
    int encontrados = 0;
    NodoProceso* temp = inicio;
    while (temp != NULL) {
        if (temp->estado == estado) {
            mostrarProceso(temp);
            encontrados++;
        }
        temp = temp->siguiente;
    }
    return encontrados;
}

// Libera todos los nodos de la lista (WHILE hasta que quede vacia)
void vaciarLista(NodoProceso*& inicio) {
    while (inicio != NULL) {
        NodoProceso* temp = inicio;
        inicio = inicio->siguiente;
        delete temp;
    }
}

/* ------------------------- GESTOR DE PROCESOS ------------------------- */

void registrarProceso() {
    string nombre;
    cin.ignore();                                    // limpia el ENTER pendiente
    cout << "  Nombre del proceso: ";
    getline(cin, nombre);
    int prioridad = leerEntero("  Prioridad (1=maxima ... 10=minima): ", 1, 10);
    int memoria = leerEntero("  Memoria requerida en KB (1-1024): ", 1, MEMORIA_TOTAL);

    insertarProceso(listaProcesos, siguienteId, nombre, prioridad, memoria);
    cout << "  [OK] Proceso registrado con ID " << siguienteId << "." << endl;
    siguienteId++;
}

void quitarProceso() {
    int id = leerEntero("  ID del proceso a eliminar: ", 1, 999999);
    if (buscarPorID(listaProcesos, id) == NULL) {
        cout << "  [!] No existe ese ID." << endl;
        return;
    }
    eliminarDeCola(frenteCPU, id);                              // sale de la cola de CPU
    int liberado = liberarBloquesDeProceso(cimaMemoria, id);    // devuelve su memoria
    eliminarProceso(listaProcesos, id);
    cout << "  [OK] Proceso eliminado. Memoria liberada: " << liberado << " KB." << endl;
}

void buscarProceso() {
    cout << "  1. Buscar por ID" << endl;
    cout << "  2. Buscar por nombre" << endl;
    int op = leerEntero("  Opcion: ", 1, 2);

    if (op == 1) {
        int id = leerEntero("  ID: ", 1, 999999);
        NodoProceso* nodo = buscarPorID(listaProcesos, id);
        if (nodo != NULL) {
            mostrarProceso(nodo);
        } else {
            cout << "  [!] No encontrado." << endl;
        }
    } else {
        string nombre;
        cin.ignore();
        cout << "  Nombre exacto: ";
        getline(cin, nombre);
        if (buscarPorNombre(listaProcesos, nombre) == 0) {
            cout << "  [!] Sin coincidencias." << endl;
        }
    }
}

void modificarPrioridad() {
    int id = leerEntero("  ID del proceso: ", 1, 999999);
    NodoProceso* nodo = buscarPorID(listaProcesos, id);
    if (nodo == NULL) {
        cout << "  [!] No existe ese ID." << endl;
        return;
    }
    int nueva = leerEntero("  Nueva prioridad (1-10): ", 1, 10);
    nodo->prioridad = nueva;

    // Si el proceso esta en la cola, se reubica con su nueva prioridad
    if (estaEnCola(frenteCPU, id)) {
        eliminarDeCola(frenteCPU, id);
        encolar(frenteCPU, id, nodo->nombre, nueva);
    }
    cout << "  [OK] Prioridad actualizada." << endl;
}

// Pide un estado y muestra los procesos que estan en ese estado
void listarPorEstadoMenu() {
    cout << "  1. Nuevo" << endl;
    cout << "  2. En cola" << endl;
    cout << "  3. Ejecutando" << endl;
    cout << "  4. Terminado" << endl;
    int op = leerEntero("  Estado a consultar: ", 1, 4);

    string estado;
    switch (op) {
        case 1: estado = "Nuevo"; break;
        case 2: estado = "En cola"; break;
        case 3: estado = "Ejecutando"; break;
        case 4: estado = "Terminado"; break;
    }

    cout << endl << "  Procesos en estado '" << estado << "':" << endl;
    if (listarPorEstado(listaProcesos, estado) == 0) {
        cout << "  (Ninguno)" << endl;
    }
}

void menuProcesos() {
    int op;
    do {
        cout << endl << "--- GESTOR DE PROCESOS (Lista enlazada) ---" << endl;
        cout << " 1. Registrar proceso" << endl;
        cout << " 2. Eliminar proceso" << endl;
        cout << " 3. Buscar proceso" << endl;
        cout << " 4. Modificar prioridad" << endl;
        cout << " 5. Listar procesos" << endl;
        cout << " 6. Listar procesos por estado" << endl;
        cout << " 0. Volver" << endl;
        op = leerEntero("Opcion: ", 0, 6);

        switch (op) {
            case 1: registrarProceso(); break;
            case 2: quitarProceso(); break;
            case 3: buscarProceso(); break;
            case 4: modificarPrioridad(); break;
            case 5:
                cout << endl << "  Procesos registrados:" << endl;
                listarProcesos(listaProcesos);
                break;
            case 6:
                listarPorEstadoMenu();
                break;
        }
    } while (op != 0);
}
