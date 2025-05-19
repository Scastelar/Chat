#include "cola.h"
#include <QDebug>

Cola::Cola(int capacidad) : capacidadMaxima(capacidad) {}

void Cola::encolar(const QStringList &elemento) {
    if (estaLlena()) {
        qWarning("La cola está llena. No se puede encolar más elementos.");
        return;
    }
    elementos.append(elemento);
}

QStringList Cola::desencolar() {
    if (estaVacia()) {
        qWarning("La cola está vacía. No se puede desencolar.");
        return QStringList();
    }
    return elementos.takeFirst();
}

QStringList Cola::frente() const {
    if (estaVacia()) {
        qWarning("La cola está vacía. No hay frente.");
        return QStringList();
    }
    return elementos.first();
}

bool Cola::estaVacia() const {
    return elementos.isEmpty();
}

int Cola::tamanio() const {
    return elementos.size();
}

void Cola::limpiar() {
    elementos.clear();
}

bool Cola::estaLlena() const {
    return tamanio() >= capacidadMaxima;
}

void Cola::setCapacidad(int nuevaCapacidad) {
    capacidadMaxima = nuevaCapacidad;
    // Si la nueva capacidad es menor que el tamaño actual, truncamos
    while (tamanio() > capacidadMaxima) {
        elementos.removeLast();
    }
}

int Cola::getCapacidad() const {
    return capacidadMaxima;
}

bool Cola::guardarEnArchivo(const QString &rutaArchivo) const {
    QFile archivo(rutaArchivo);
    if (!archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "No se pudo abrir el archivo para escribir:" << rutaArchivo;
        return false;
    }

    QTextStream out(&archivo);
    for (const QStringList &elemento : elementos) {
        out << elemento.join("|||") << "\n"; // Usamos ||| como separador
    }
    archivo.close();
    return true;
}

bool Cola::cargarDesdeArchivo(const QString &rutaArchivo) {
    QFile archivo(rutaArchivo);
    if (!archivo.exists()) {
        return false; // No es error si el archivo no existe
    }

    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "No se pudo abrir el archivo para leer:" << rutaArchivo;
        return false;
    }

    limpiar();
    QTextStream in(&archivo);
    while (!in.atEnd()) {
        QString linea = in.readLine().trimmed();
        if (!linea.isEmpty()) {
            elementos.append(linea.split("|||"));
        }
    }
    archivo.close();
    return true;
}
