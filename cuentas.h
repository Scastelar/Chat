#ifndef CUENTAS_H
#define CUENTAS_H

#include "Usuario.h"
#include <QString>
#include <Qvector>
#include <QFile>
#include <QTextStream>

class Cuentas
{
private:
    QString archivo;

public:
    Cuentas(const QString& archivoNombre)
        : archivo(archivoNombre) {}

    // Metodos
    bool isUsuarioUnico(const QString& user);
    void escribirUsuario(const Usuario& usuario);
    bool iniciarSesion(const QString& nombreUsuario, const QString& contrasena);
    void crearUsuario(QString nombre, QString contrasena);
    void eliminarUsuario(const QString& nombre);

    void leerUsuarios(QVector<Usuario>& usuarios);
};

#endif // CUENTAS_H
