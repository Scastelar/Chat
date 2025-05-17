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
                      int edad, QString password, QString respuesta, QString avatarPath, bool est);
    void eliminarContacto(const QString& nombre, const QString& archivo);
    void leerUsuarios(QVector<Usuario>& usuarios);
   bool getUsuarioPorNombre(const QString& username, Usuario& usuarioEncontrado);
    void leerContactos(QVector<QString>& contactos);
    bool actualizarEstadoUsuario(const QString& username, bool estado);
};

#endif // CUENTAS_H
