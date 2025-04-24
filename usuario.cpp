#include "usuario.h"

Usuario::Usuario(QString user, QString pass)
    : username(user),password(pass) {
}

Usuario::Usuario() {}

bool Usuario::validarContrasena(const QString& pass) const {
    return password == pass;
}
