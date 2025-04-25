#include "Cuentas.h"
#include "Usuario.h"
#include <QDebug>

Cuentas::Cuentas(QString archivoNombre)
{
    archivo = archivoNombre;

    QFile file(archivo);
    if (!file.exists()) {
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << ""; // Archivo en blanco
            file.close();
        } else {
            qDebug() << "No se pudo crear el archivo:" << archivo;
        }
    }
}


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
        out << usuario.getUsername().toLower() << ";"
            << usuario.getPassword().toLower() << ";"
            << usuario.getNombre() << ";"
            << usuario.getCorreo() << ";"
            << usuario.getPregunta() << ";"
            << usuario.getResp() << ";"
            << usuario.getEdad() << ";"
            << usuario.getPerfil() << "\n";
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
            QString line = in.readLine();
            QStringList parts = line.split(";");
            if (parts.size() >= 8) {
                QString username = parts[0];
                QString password = parts[1];
                QString nombre = parts[2];
                QString correo = parts[3];
                QString pregunta = parts[4];
                QString respuesta = parts[5];
                int edad = parts[6].toInt();
                QString avatarPath = parts[7];

                // Elimina estas líneas que están de más:
                // in >> username >> password >> nombre >> correo >> pregunta >> respuesta >> edad >> avatarPath;

                if (!username.isEmpty() && !password.isEmpty()) {
                    Usuario u(username.toLower(), password.toLower());
                    u.setNombre(nombre);
                    u.setCorreo(correo);
                    u.setPregunta(pregunta);
                    u.setResp(respuesta);
                    u.setEdad(edad);
                    u.setPerfil(avatarPath);
                    usuarios.append(u);
                }
                // Elimina esta línea que está de más:
                // in.readLine(); // Para evitar saltos incorrectos de línea
            }
        }
        file.close();
    } else {
        qWarning() << "Error al abrir el archivo para lectura.";
    }
}

bool Cuentas::iniciarSesion(const QString& user, const QString& pass, Usuario& usuarioEncontrado) {
    QFile file(archivo);  // Usa archivo en lugar de "cuentas.txt" directamente
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(";");
            if (parts.size() >= 8) {
                QString username = parts[0];
                QString password = parts[1];
                QString nombre = parts[2];
                QString correo = parts[3];
                QString pregunta = parts[4];
                QString respuesta = parts[5];
                int edad = parts[6].toInt();
                QString avatarPath = parts[7];

                // Elimina estas líneas que están de más:
                // in >> username >> password >> nombre >> correo >> pregunta >> respuesta >> edad >> avatarPath;

                if (username == user.toLower() && password == pass.toLower()) {
                    usuarioEncontrado = Usuario(username, password);
                    usuarioEncontrado.setNombre(nombre);
                    usuarioEncontrado.setCorreo(correo);
                    usuarioEncontrado.setPregunta(pregunta);
                    usuarioEncontrado.setResp(respuesta);
                    usuarioEncontrado.setEdad(edad);
                    usuarioEncontrado.setPerfil(avatarPath);
                    file.close();
                    return true;
                }
                // Elimina esta línea que está de más:
                // in.readLine();
            }
        }
        file.close();
    }
    return false;
}


/*
void Cuentas::crearUsuario(QString nombre, QString username, QString correo, QString pregunta,
                           int edad, QString password, QString respuesta, QString avatarPath)
{
    if (isUsuarioUnico(username)){
    QFile file("cuentas.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << username.toLower() << ";"
            << password.toLower() << ";"
            << nombre << ";"
            << correo << ";"
            << pregunta << ";"
            << respuesta << ";"
            << edad << ";"
            << avatarPath << "\n";
        file.close();
    }
    }
}*/

void Cuentas::crearUsuario(QString nombre, QString username, QString correo, QString pregunta,
                           int edad, QString password, QString respuesta, QString avatarPath) {
    if (isUsuarioUnico(username)) {
        Usuario nuevo(username, password);
        nuevo.setNombre(nombre);
        nuevo.setCorreo(correo);
        nuevo.setPregunta(pregunta);
        nuevo.setResp(respuesta);
        nuevo.setEdad(edad);
        nuevo.setPerfil(avatarPath);
        escribirUsuario(nuevo);
    }
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
                out << usuario.getUsername() << ";"
                    << usuario.getPassword() << ";"
                    << usuario.getNombre() << ";"
                    << usuario.getCorreo() << ";"
                    << usuario.getPregunta() << ";"
                    << usuario.getResp() << ";"
                    << usuario.getEdad() << ";"
                    << usuario.getPerfil() << "\n";
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
