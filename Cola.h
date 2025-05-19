#ifndef COLA_H
#define COLA_H

#include <QList>
#include <QStringList>
#include <QFile>
#include <QTextStream>

class Cola {
private:
    QList<QStringList> elementos;
    int capacidadMaxima;

public:
    explicit Cola(int capacidad = 1000);

    // Métodos básicos
    void encolar(const QStringList &elemento);
    QStringList desencolar();
    QStringList frente() const;
    bool estaVacia() const;
    int tamanio() const;
    void limpiar();

    // Persistencia
    bool guardarEnArchivo(const QString &rutaArchivo) const;
    bool cargarDesdeArchivo(const QString &rutaArchivo);

    // Capacidad
    bool estaLlena() const;
    void setCapacidad(int nuevaCapacidad);
    int getCapacidad() const;
};

#endif // COLA_H
