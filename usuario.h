#ifndef USUARIO_H
#define USUARIO_H

#include <string>
using namespace std;

class Usuario
{
public:
    Usuario(string,string);
    string getNombre(){
        return nombre;
    }
    string getUsername(){
        return username;
    }
    string getPassword(){
        return password;
    }
    int getEdad(){
        return edad;
    }
    string getCorreo(){
        return correo;
    }

    string getPregunta(){
        return pregunta;
    }
    string getResp(){
        return respuesta;
    }

    void setUser(string user){
        username=username;
    }

    void setPassword(string pass){
        password=pass;
    }

    bool validarContrasena(const string& pass) const;

private:
    string nombre;
    string username;
    string password;
    int edad;
    string correo;
    string pregunta;
    string respuesta;

};

#endif // USUARIO_H
