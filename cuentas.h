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
    explicit Cuentas(QString archivoNombre);

    // Metodos
    bool isUsuarioUnico(const QString& user);
    void escribirUsuario(const Usuario& usuario);
    bool iniciarSesion(const QString& user, const QString& pass, Usuario& usuarioEncontrado);
    void crearUsuario(QString nombre, QString username, QString correo, QString pregunta,
                      int edad, QString password, QString respuesta, QString avatarPath);
    void eliminarUsuario(const QString& nombre);
    void leerUsuarios(QVector<Usuario>& usuarios);
};

#endif // CUENTAS_H
