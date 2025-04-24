#include "Cuentas.h"
#include "Usuario.h"
#include <QDebug>


bool Cuentas::isUsuarioUnico(const QString& user){
    QVector<Usuario> usuarios;
    leerUsuarios(usuarios);

    QString lowerUser = user.toLower();

    for (const auto& u : usuarios) {
        if (u.getUsername().toLower() == lowerUser) {
            return false; // Usuario ya existe
        }
    }
    return true;
}

void Cuentas::escribirUsuario(const Usuario& usuario) {
    QFile file(archivo);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << usuario.getNombre() << " "
            << usuario.getUsername().toLower() << " "
            << usuario.getPassword().toLower() << " "
            << usuario.getEdad() << " "
            << usuario.getCorreo() << " "
            << usuario.getPregunta() << " "
            << usuario.getResp() << "\n";
        file.close();
    } else {
        qWarning() << "Error al abrir el archivo para escritura.";
    }
}

void Cuentas::leerUsuarios(QVector<Usuario>& usuarios) {
    QFile file(archivo);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString nombre, username, password, correo, pregunta, respuesta;
            int edad;
            in >> nombre >> username >> password >> edad >> correo >> pregunta >> respuesta;
            if (!username.isEmpty() && !password.isEmpty()) {
                Usuario u;
                u.setNombre(nombre);
                u.setUser(username.toLower());
                u.setPassword(password.toLower());
                u.setEdad(edad);
                u.setCorreo(correo);
                u.setPregunta(pregunta);
                u.setResp(respuesta);
                usuarios.append(u);
            }
        }
        file.close();
    } else {
        qWarning() << "Error al abrir el archivo para lectura.";
    }
}

bool Cuentas::iniciarSesion(const QString& nombreUsuario, const QString& contrasena) {
    QVector<Usuario> usuarios;
    leerUsuarios(usuarios);

    QString lowerUser = nombreUsuario.toLower();
    QString lowerPass = contrasena.toLower();

    for (const auto& u : usuarios) {
        if (u.getUsername().toLower() == lowerUser && u.getPassword().toLower() == lowerPass) {
            return true;
        }
    }
    return false;
}

void Cuentas::crearUsuario(QString nombre, QString contrasena) {
    QString lowerNombre = nombre.toLower();
    QString lowerContrasena = contrasena.toLower();

    if (!isUsuarioUnico(lowerNombre)) {
        qDebug() << "El nombre de usuario ya existe. Intente nuevamente.";
        return;
    }

    Usuario nuevoUsuario;
    nuevoUsuario.setUser(lowerNombre);
    nuevoUsuario.setPassword(lowerContrasena);
    escribirUsuario(nuevoUsuario);
    qDebug() << "Usuario registrado con éxito.";
}

void Cuentas::eliminarUsuario(const QString& nombre) {
    QVector<Usuario> usuarios;
    leerUsuarios(usuarios);

    QFile temp("temp.txt");
    bool encontrado = false;
    QString lowerNombre = nombre.toLower();

    if (temp.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&temp);
        for (const auto& usuario : usuarios) {
            if (usuario.getUsername().toLower() != lowerNombre) {
                out << usuario.getNombre() << " "
                    << usuario.getUsername() << " "
                    << usuario.getPassword() << " "
                    << usuario.getEdad() << " "
                    << usuario.getCorreo() << " "
                    << usuario.getPregunta() << " "
                    << usuario.getResp() << "\n";
            } else {
                encontrado = true;
            }
        }
        temp.close();
        QFile::remove(archivo);
        QFile::rename("temp.txt", archivo);

        if (encontrado) {
            qDebug() << "Usuario eliminado con éxito.";
        } else {
            qDebug() << "Usuario no encontrado.";
        }
    } else {
        qWarning() << "Error al abrir el archivo temporal para escritura.";
    }
}
