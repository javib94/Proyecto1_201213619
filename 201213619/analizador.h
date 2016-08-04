#ifndef ANALIZADOR_H_INCLUDED
#define ANALIZADOR_H_INCLUDED

typedef struct NodoLista{
    char parametro[200];
    char nombre[200];
    struct NodoLista* siguiente;
}NodoLista;

typedef struct Lista{
    NodoLista *primero;
}Lista;

void ejecutar_lista(Lista *lista);
void insertar_parametro(Lista *lista, char *nombre, char*parametro);
void EJECUTAR_COMANDO(char *comando);
void EXEC(char *archivo);










#endif
