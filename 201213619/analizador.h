#ifndef ANALIZADOR_H_INCLUDED
#define ANALIZADOR_H_INCLUDED


//tipos te token: comando=1, parametro=2, igual=3, cadena=4, direccion=5
struct token{
    char cadena[250];
    int tipo;
}token;

void analizar_cadena(char* cadena);













#endif
