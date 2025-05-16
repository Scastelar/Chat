#ifndef USUARIO_H
#define USUARIO_H

#include <QString>

class Usuario
{
public:
    Usuario();
    Usuario(QString nombre, QString username);

    QString getNombre() const { return nombre; }
    QString getUsername() const { return username; }
    QString getPassword() const { return password; }
    int getEdad() const { return edad; }
    QString getCorreo() const { return correo; }
    QString getPregunta() const { return pregunta; }
    QString getResp() const { return respuesta; }
    QString getPerfil() const { return perfil; }
   bool getEstado() const { return estado; }

    void setUser(QString user) { username = user; }
    void setPassword(QString pass) { password = pass; }
    void setNombre(QString name){ nombre = name; }
    void setEdad(int age){ edad = age; }
    void setCorreo(QString email){ correo = email; }
    void setPregunta(QString question) { pregunta = question; }
    void setResp(QString answer){ respuesta = answer;}
    void setPerfil(QString avatar) { perfil = avatar; }
    void setEstado(bool est) { estado = est; }

    bool validarContrasena(const QString& pass) const;

private:
    QString nombre;
    QString username;
    QString password;
    QString correo;
    QString pregunta;
    QString respuesta;
    int edad = 0;
    QString perfil;
    bool estado = 0;
};

#endif // USUARIO_H
