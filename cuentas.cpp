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

bool Cuentas::getUsuarioPorNombre(const QString& username, Usuario& usuarioEncontrado)
{
    QFile file(archivo);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error al abrir cuentas.txt";
        return false;
    }

    QString usernameLower = username.toLower().trimmed();
    qDebug() << "Buscando usuario:" << usernameLower;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if(line.isEmpty()) continue;

        QStringList parts = line.split(";");
        if(parts.size() < 8) {
            qDebug() << "Línea mal formada:" << line;
            continue;
        }

        QString storedUser = parts[0].toLower().trimmed();
        qDebug() << "Usuario en archivo:" << storedUser;

        if (storedUser == usernameLower) {
            usuarioEncontrado = Usuario(parts[0], parts[1]);
            usuarioEncontrado.setNombre(parts[2]);
            usuarioEncontrado.setCorreo(parts[3]);
            usuarioEncontrado.setPregunta(parts[4]);
            usuarioEncontrado.setResp(parts[5]);
            usuarioEncontrado.setEdad(parts[6].toInt());
            usuarioEncontrado.setPerfil(parts[7]);

            if(parts.size() >= 9) {
                usuarioEncontrado.setEstado(parts[8].toInt());
            } else {
                usuarioEncontrado.setEstado(0);
            }

            file.close();
            qDebug() << "Usuario encontrado! Estado:" << usuarioEncontrado.getEstado();
            return true;
        }
    }
    file.close();
    qDebug() << "Usuario no encontrado en archivo";
    return false;
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
            << usuario.getPerfil() << ";"
            << usuario.getEstado() << "\n";
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
            }
        }
        file.close();
    } else {
        qWarning() << "Error al abrir el archivo para lectura.";
    }
}
void Cuentas::leerContactos(QVector<QString>& contactos) {
    QFile file(archivo);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString username = in.readLine().trimmed();
            if (!username.isEmpty()) {
                contactos.append(username);
            }
        }
        file.close();
    } else {
        qWarning() << "Error al abrir el archivo para lectura:" << archivo;
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



void Cuentas::crearUsuario(QString nombre, QString username, QString correo, QString pregunta,
                           int edad, QString password, QString respuesta, QString avatarPath,bool est) {
    if (isUsuarioUnico(username)) {
        Usuario nuevo(username, password);
        nuevo.setNombre(nombre);
        nuevo.setCorreo(correo);
        nuevo.setPregunta(pregunta);
        nuevo.setResp(respuesta);
        nuevo.setEdad(edad);
        nuevo.setPerfil(avatarPath);
        nuevo.setEstado(est);
        escribirUsuario(nuevo);
    }
}


void Cuentas::eliminarContacto(const QString& nombre, const QString& archivo) {
    // 1. Leer TODOS los contactos actuales
    QVector<QString> contactos;
    QFile file(archivo);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString contacto = in.readLine().trimmed();
            if (!contacto.isEmpty()) {
                contactos.append(contacto);
            }
        }
        file.close();
    }

    // 2. Volver a abrir el archivo para escritura (esto lo SOBREESCRIBE)
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        bool encontrado = false;

        // 3. Escribir solo los contactos que NO coinciden
        for (const QString& contacto : contactos) {
            if (contacto.toLower() != nombre.toLower()) {
                out << contacto << "\n";
            } else {
                encontrado = true;
            }
        }

        file.close();

        if (encontrado) {
            qDebug() << "Contacto eliminado:" << nombre;
        } else {
            qDebug() << "Contacto no encontrado:" << nombre;
        }
    } else {
        qWarning() << "Error al abrir archivo para escritura:" << archivo;
    }
}

bool Cuentas::actualizarEstadoUsuario(const QString& username, bool estado)
{
    // Leer todo el archivo
    QFile file(archivo);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error al abrir cuentas.txt para lectura";
        return false;
    }

    QStringList lines;
    QTextStream in(&file);
    bool usuarioEncontrado = false;
    QString usernameLower = username.toLower().trimmed();

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if(line.isEmpty()) continue;

        QStringList parts = line.split(";");
        if(parts.size() < 8) continue;

        QString storedUser = parts[0].toLower().trimmed();

        if (storedUser == usernameLower) {
            // Reconstruir la línea con el nuevo estado
            QString newLine = parts.join(";");
            if(parts.size() >= 9) {
                // Reemplazar el estado existente
                newLine = parts.mid(0, 8).join(";") + ";" + QString::number(estado);
            } else {
                // Añadir el estado
                newLine += ";" + QString::number(estado);
            }
            lines << newLine;
            usuarioEncontrado = true;
        } else {
            lines << line;
        }
    }
    file.close();

    if(!usuarioEncontrado) {
        qDebug() << "Usuario no encontrado para actualizar estado";
        return false;
    }

    // Escribir todo el archivo de nuevo
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Error al abrir cuentas.txt para escritura";
        return false;
    }

    QTextStream out(&file);
    foreach(const QString &line, lines) {
        out << line << "\n";
    }
    file.close();

    return true;
}
