/*
 * SISTEMA DE GESTION DE PROCESOS - Estructura de Datos (Unidad 3)
 * Lenguaje: C++ (Dev C++)
 *
 * Estructuras dinamicas lineales hechas desde cero:
 *   1. Lista enlazada simple -> Gestor de Procesos
 *   2. Cola de prioridad     -> Planificador de CPU
 *   3. Pila                  -> Gestor de Memoria
 *
 * Prioridad: 1 = maxima ... 10 = minima
 */

#include <iostream>
#include <fstream>    // SOLO para la persistencia (guardar/cargar), exigida por la consigna
#include <string>
using namespace std;

const int MEMORIA_TOTAL = 1024;   // KB disponibles en el sistema

/* ===================================================================== */
/*  1. LISTA ENLAZADA SIMPLE - GESTOR DE PROCESOS                        */
/* ===================================================================== */

// Cada nodo guarda un proceso y un puntero al siguiente nodo
struct NodoProceso {
    int id;
    string nombre;
    int prioridad;
    int memoriaKB;
    string estado;          // "Nuevo", "En cola", "Terminado"
    NodoProceso* siguiente;
};

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

/* ===================================================================== */
/*  2. COLA DE PRIORIDAD - PLANIFICADOR DE CPU                           */
/*     Lista ordenada por prioridad: el frente es el de mayor prioridad  */
/* ===================================================================== */

struct NodoCola {
    int idProceso;
    string nombre;
    int prioridad;
    NodoCola* siguiente;
};

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

/* ===================================================================== */
/*  3. PILA - GESTOR DE MEMORIA                                          */
/*     Cada nodo es un bloque de memoria asignado a un proceso          */
/* ===================================================================== */

struct NodoBloque {
    int idBloque;
    int idProceso;
    int tamKB;
    NodoBloque* siguiente;
};

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

/* ===================================================================== */
/*  PROGRAMA PRINCIPAL                                                   */
/* ===================================================================== */

// Las tres estructuras empiezan vacias (NULL)
NodoProceso* listaProcesos = NULL;   // inicio de la lista
NodoCola* frenteCPU = NULL;          // frente de la cola de prioridad
NodoBloque* cimaMemoria = NULL;      // cima de la pila de memoria
int siguienteId = 1;                 // ID automatico para procesos
int siguienteBloque = 1;             // numero automatico para bloques

/* --------------------------- PERSISTENCIA ----------------------------- */
// El estado del sistema se guarda en un archivo de texto con este orden:
//   siguienteId siguienteBloque
//   cantidad de procesos, y por cada uno: id / nombre / prioridad memoria / estado
//   cantidad en cola, y por cada uno: id (de frente a fondo)
//   cantidad de bloques, y por cada uno: idBloque idProceso tamKB (de cima a fondo)

const char* ARCHIVO = "sistema_procesos.txt";

// Liberan todos los nodos de una estructura (WHILE hasta que quede vacia)
void vaciarLista(NodoProceso*& inicio) {
    while (inicio != NULL) {
        NodoProceso* temp = inicio;
        inicio = inicio->siguiente;
        delete temp;
    }
}

void vaciarCola(NodoCola*& frente) {
    while (frente != NULL) {
        NodoCola* temp = frente;
        frente = frente->siguiente;
        delete temp;
    }
}

void vaciarPila(NodoBloque*& cima) {
    while (cima != NULL) {
        NodoBloque* temp = cima;
        cima = cima->siguiente;
        delete temp;
    }
}

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
