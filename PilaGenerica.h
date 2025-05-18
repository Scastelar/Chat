#ifndef PILAGENERICA_H
#define PILAGENERICA_H

#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>

template <class T>
class PilaGenerica {
private:
    class NodoPila {
    public:
        T elemento;
        NodoPila* siguiente;
        NodoPila(T item) : elemento(item), siguiente(nullptr) {}
    };

    NodoPila* cima;

public:
    PilaGenerica() : cima(nullptr) {}
    void insertar(T elemento);
    T quitar();
    T cimaPila() const;
    bool pilaVacia() const;
    void limpiarPila();
    int size() const;
    void listar() const;
    void guardarEnArchivo(const std::string& nombreArchivo) const;
    void cargarDesdeArchivo(const std::string& nombreArchivo);

    ~PilaGenerica() {
        limpiarPila();
    }
};

template <class T>
bool PilaGenerica<T>::pilaVacia() const {
    return cima == nullptr;
}

template <class T>
void PilaGenerica<T>::insertar(T elemento) {
    NodoPila* nuevo = new NodoPila(elemento);
    nuevo->siguiente = cima;
    cima = nuevo;
}

template <class T>
T PilaGenerica<T>::quitar() {
    if (pilaVacia()) {
        throw std::runtime_error("Pila vacia, no se puede extraer");
    }

    T temp = cima->elemento;
    NodoPila* nodoEliminar = cima;
    cima = cima->siguiente;
    delete nodoEliminar;
    return temp;
}

template <class T>
T PilaGenerica<T>::cimaPila() const {
    if (pilaVacia()) {
        throw std::runtime_error("Pila vacia");
    }
    return cima->elemento;
}

template <class T>
void PilaGenerica<T>::limpiarPila() {
    while (!pilaVacia()) {
        NodoPila* nodoEliminar = cima;
        cima = cima->siguiente;
        delete nodoEliminar;
    }
}

template <class T>
int PilaGenerica<T>::size() const {
    int contador = 0;
    NodoPila* actual = cima;
    while (actual != nullptr) {
        contador++;
        actual = actual->siguiente;
    }
    return contador;
}

template <class T>
void PilaGenerica<T>::listar() const {
    if (pilaVacia()) {
        std::cout << "La pila esta vacia" << std::endl;
        return;
    }
    NodoPila* actual = cima;
    std::cout << "Elementos en la pila: " << std::endl;
    while (actual != nullptr) {
        std::cout << actual->elemento << std::endl;
        actual = actual->siguiente;
    }
}

template <class T>
void PilaGenerica<T>::guardarEnArchivo(const std::string& nombreArchivo) const {
    std::ofstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo para escritura");
    }

    NodoPila* actual = cima;
    while (actual != nullptr) {
        // Conversión específica para QString
        if constexpr (std::is_same_v<T, QString>) {
            archivo << actual->elemento.toStdString() << "\n";
        } else {
            archivo << actual->elemento << "\n";
        }
        actual = actual->siguiente;
    }
    archivo.close();
}

template <class T>
void PilaGenerica<T>::cargarDesdeArchivo(const std::string& nombreArchivo) {
    limpiarPila();
    std::ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        return;
    }

    std::string linea;
    std::vector<T> elementos;
    while (std::getline(archivo, linea)) {
        if (!linea.empty()) {
            // Conversión específica para QString
            if constexpr (std::is_same_v<T, QString>) {
                elementos.push_back(QString::fromStdString(linea));
            } else {
                elementos.push_back(linea);
            }
        }
    }
    archivo.close();

    for (auto it = elementos.rbegin(); it != elementos.rend(); ++it) {
        insertar(*it);
    }
}

#endif // PILAGENERICA_H
