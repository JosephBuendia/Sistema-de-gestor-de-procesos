/*
 * SISTEMA DE GESTION DE PROCESOS - Estructura de Datos (Unidad 3)
 * Lenguaje: C++ (Dev C++)
 *
 * Estructuras dinamicas lineales hechas desde cero:
 *   1. Lista enlazada simple -> Gestor de Procesos   (procesos.h / procesos.cpp)
 *   2. Cola de prioridad     -> Planificador de CPU  (cpu.h / cpu.cpp)
 *   3. Pila                  -> Gestor de Memoria    (memoria.h / memoria.cpp)
 *
 * Este archivo contiene: lectura de datos, persistencia y menu principal.
 * Prioridad: 1 = maxima ... 10 = minima
 */

#include <iostream>
#include <fstream>    // SOLO para la persistencia (guardar/cargar), exigida por la consigna
#include <string>
#include "common.h"
#include "procesos.h"
#include "cpu.h"
#include "memoria.h"
using namespace std;

/* ===================================================================== */
/*  ENTRADA DE DATOS                                                     */
/* ===================================================================== */

// Pide un entero y repite (WHILE) hasta que este entre minimo y maximo
int leerEntero(string mensaje, int minimo, int maximo) {
    int valor;
    cout << mensaje;
    cin >> valor;
    while (valor < minimo || valor > maximo) {
        cout << "  Valor invalido. Debe estar entre " << minimo << " y " << maximo << "." << endl;
        cout << mensaje;
        cin >> valor;
    }
    return valor;
}

/* --------------------------- PERSISTENCIA ----------------------------- */
// El estado del sistema se guarda en un archivo de texto con este orden:
//   siguienteId siguienteBloque
//   cantidad de procesos, y por cada uno: id / nombre / prioridad memoria / estado
//   cantidad en cola, y por cada uno: id (de frente a fondo)
//   cantidad de bloques, y por cada uno: idBloque idProceso tamKB (de cima a fondo)

const char* ARCHIVO = "sistema_procesos.txt";

// Escribe todo el estado actual en el archivo. Devuelve false si no pudo abrirlo
bool guardarEstado() {
    ofstream archivo(ARCHIVO);
    if (!archivo) {
        return false;
    }

    archivo << siguienteId << " " << siguienteBloque << endl;

    // Procesos: primero cuantos son, luego cada uno
    int cantidad = 0;
    NodoProceso* p = listaProcesos;
    while (p != NULL) {
        cantidad++;
        p = p->siguiente;
    }
    archivo << cantidad << endl;
    p = listaProcesos;
    while (p != NULL) {
        archivo << p->id << endl;
        archivo << p->nombre << endl;
        archivo << p->prioridad << " " << p->memoriaKB << endl;
        archivo << p->estado << endl;
        p = p->siguiente;
    }

    // Cola de CPU: solo se guarda el ID, lo demas se recupera de la lista
    cantidad = 0;
    NodoCola* c = frenteCPU;
    while (c != NULL) {
        cantidad++;
        c = c->siguiente;
    }
    archivo << cantidad << endl;
    c = frenteCPU;
    while (c != NULL) {
        archivo << c->idProceso << endl;
        c = c->siguiente;
    }

    // Pila de memoria: de la cima al fondo
    cantidad = 0;
    NodoBloque* b = cimaMemoria;
    while (b != NULL) {
        cantidad++;
        b = b->siguiente;
    }
    archivo << cantidad << endl;
    b = cimaMemoria;
    while (b != NULL) {
        archivo << b->idBloque << " " << b->idProceso << " " << b->tamKB << endl;
        b = b->siguiente;
    }

    archivo.close();
    return true;
}

// Descarta lo leido a medias cuando el archivo esta danado
bool cargaFallida(NodoProceso*& lista, NodoCola*& cola, NodoBloque*& pila) {
    vaciarLista(lista);
    vaciarCola(cola);
    vaciarPila(pila);
    return false;
}

// Lee el archivo y reconstruye las tres estructuras.
// Se arma todo en estructuras nuevas y solo si sale bien reemplaza el estado actual.
bool cargarEstado() {
    ifstream archivo(ARCHIVO);
    if (!archivo) {
        return false;
    }

    NodoProceso* nuevaLista = NULL;
    NodoCola* nuevaCola = NULL;
    NodoBloque* nuevaPila = NULL;
    int sigId, sigBloque, cantidad;

    archivo >> sigId >> sigBloque >> cantidad;
    if (archivo.fail()) {
        return cargaFallida(nuevaLista, nuevaCola, nuevaPila);
    }
    archivo.ignore();                       // salta el ENTER antes del primer nombre

    // Procesos
    for (int i = 0; i < cantidad; i++) {
        int id, prioridad, memoria;
        string nombre, estado;
        archivo >> id;
        archivo.ignore();
        getline(archivo, nombre);
        archivo >> prioridad >> memoria;
        archivo.ignore();
        getline(archivo, estado);
        if (archivo.fail()) {
            return cargaFallida(nuevaLista, nuevaCola, nuevaPila);
        }
        insertarProceso(nuevaLista, id, nombre, prioridad, memoria);
        buscarPorID(nuevaLista, id)->estado = estado;   // insertarProceso pone "Nuevo"
    }

    // Cola: se vuelve a encolar en el mismo orden, la prioridad la ordena sola
    archivo >> cantidad;
    for (int i = 0; i < cantidad; i++) {
        int id;
        archivo >> id;
        if (archivo.fail()) {
            return cargaFallida(nuevaLista, nuevaCola, nuevaPila);
        }
        NodoProceso* nodo = buscarPorID(nuevaLista, id);
        if (nodo != NULL) {
            encolar(nuevaCola, nodo->id, nodo->nombre, nodo->prioridad);
        }
    }

    // Pila: el archivo viene de cima a fondo, asi que se apila en una pila
    // temporal (queda al reves) y luego se pasa a la definitiva (queda en orden)
    archivo >> cantidad;
    NodoBloque* pilaTemporal = NULL;
    for (int i = 0; i < cantidad; i++) {
        int idB, idP, tam;
        archivo >> idB >> idP >> tam;
        if (archivo.fail()) {
            vaciarPila(pilaTemporal);
            return cargaFallida(nuevaLista, nuevaCola, nuevaPila);
        }
        apilar(pilaTemporal, idB, idP, tam);
    }
    int idB, idP, tam;
    while (desapilar(pilaTemporal, idB, idP, tam)) {
        apilar(nuevaPila, idB, idP, tam);
    }

    archivo.close();

    // Todo salio bien: se descarta el estado actual y se reemplaza
    vaciarLista(listaProcesos);
    vaciarCola(frenteCPU);
    vaciarPila(cimaMemoria);
    listaProcesos = nuevaLista;
    frenteCPU = nuevaCola;
    cimaMemoria = nuevaPila;
    siguienteId = sigId;
    siguienteBloque = sigBloque;
    return true;
}

/* ---------------------------- MENU PRINCIPAL -------------------------- */

int main() {
    int op;

    // Al iniciar, si existe un archivo guardado se recupera el estado anterior
    if (cargarEstado()) {
        cout << "[Sistema] Estado anterior cargado desde '" << ARCHIVO << "'." << endl;
    }

    do {
        cout << endl << "==========================================" << endl;
        cout << "   SISTEMA DE GESTION DE PROCESOS" << endl;
        cout << "==========================================" << endl;
        cout << " 1. Gestor de Procesos   (Lista enlazada)" << endl;
        cout << " 2. Planificador de CPU  (Cola de prioridad)" << endl;
        cout << " 3. Gestor de Memoria    (Pila)" << endl;
        cout << " 4. Guardar estado" << endl;
        cout << " 5. Cargar estado" << endl;
        cout << " 0. Salir" << endl;
        op = leerEntero("Opcion: ", 0, 5);

        switch (op) {
            case 1: menuProcesos(); break;
            case 2: menuCPU(); break;
            case 3: menuMemoria(); break;
            case 4:
                if (guardarEstado()) {
                    cout << "  [OK] Estado guardado." << endl;
                } else {
                    cout << "  [!] Error al guardar." << endl;
                }
                break;
            case 5:
                if (cargarEstado()) {
                    cout << "  [OK] Estado cargado." << endl;
                } else {
                    cout << "  [!] No se pudo cargar el archivo." << endl;
                }
                break;
            case 0:
                if (leerEntero("  Guardar antes de salir? (1=Si, 0=No): ", 0, 1) == 1) {
                    guardarEstado();
                }
                cout << "Hasta pronto." << endl;
                break;
        }
    } while (op != 0);

    return 0;
}
