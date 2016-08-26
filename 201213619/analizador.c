#include "analizador.h"
#include <string.h>


void ejecutar_lista(Lista *lista){
    NodoLista* aux = lista->primero;
    if(!strcasecmp(aux->nombre,"mkdisk")){
        int size=0;
        char unit= 'k';
        char path[100];
        limpiar_cadena(path,100);
        aux = aux->siguiente;
        while(aux!=NULL){
            if(!strcasecmp(aux->nombre,"size")){
                size=strtol(aux->parametro,NULL,10);
            }
            else if(!strcasecmp(aux->nombre,"unit")){
                if(!strcasecmp(aux->parametro,"k")||!strcasecmp(aux->parametro,"m")||!strcasecmp(aux->parametro,"b")){
                    unit=aux->parametro[0];
                }else{
                    printf("Unidad no valida\n");
                }
            }
            else if(!strcasecmp(aux->nombre,"path")){
                strcpy(path,aux->parametro);
            }else{
                printf("mkdisk no posee el parametro %s \n",aux->nombre);
            }
            aux=aux->siguiente;
        }
        MKDISK(size,unit,path);
    }
    else if(!strcasecmp(aux->nombre,"rmdisk")){
        char path[100];
        limpiar_cadena(path,100);
        aux = aux->siguiente;
        while(aux!=NULL){
            if(!strcasecmp(aux->nombre,"path")){
                strcpy(path,aux->parametro);
            }else{
                printf("rmdisk no posee el parametro %s \n",aux->nombre);
            }
            aux=aux->siguiente;
        }
        RMDISK(path);
    }
    else if(!strcasecmp(aux->nombre,"fdisk")){
        int size=0;//obligatorio, >0
        char unit='K';//opcional, default=k
        char path[100];//obligatorio
        limpiar_cadena(path,100);
        char type='P';
        char fit='W';
        char deletee='F';
        char name[16];
        limpiar_cadena(name,16);
        int add=0;
        aux = aux->siguiente;
        while(aux!=NULL){
            if(!strcasecmp(aux->nombre,"size")){
                size=strtol(aux->parametro,NULL,10);
            }
            else if(!strcasecmp(aux->nombre,"unit")){
                if(!strcasecmp(aux->parametro,"k")||!strcasecmp(aux->parametro,"m")||!strcasecmp(aux->parametro,"b")){
                    unit=aux->parametro[0];
                }else{
                    printf("Unidad no validas\n");
                }
            }
            else if(!strcasecmp(aux->nombre,"path")){
                strcpy(path,aux->parametro);
            }else if(!strcasecmp(aux->nombre,"type")){
                if(!strcasecmp(aux->parametro,"p")||!strcasecmp(aux->parametro,"e")||!strcasecmp(aux->parametro,"l")){
                    type=aux->parametro[0];
                }else{
                    printf("Tipo de particion no valida\n");
                }
            }else if(!strcasecmp(aux->nombre,"fit")){
                if(!strcasecmp(aux->parametro,"bf")||!strcasecmp(aux->parametro,"ff")||!strcasecmp(aux->parametro,"wf")){
                    fit=aux->parametro[0];
                }else{
                    printf("Tipo de ajuste no valido\n");
                }
            }else if(!strcasecmp(aux->nombre, "delete")){
                if(!strcasecmp(aux->parametro,"FAST")||!strcasecmp(aux->parametro,"FULL")){
                    deletee=aux->parametro[1];
                }else{
                    printf("Tipo de borrado no valido\n");
                }
            }else if(!strcasecmp(aux->nombre, "add")){
                add=strtol(aux->parametro,NULL,10);
            }else if(!strcasecmp(aux->nombre, "name")){
                if(strlen(aux->parametro)>0){
                    strcpy(name, aux->parametro);
                }
            }
            else{
                printf("fdisk no posee el parametro %s \n",aux->nombre);
            }
            aux=aux->siguiente;
        }
        FDISK(size, unit, path, type, fit, deletee, add, name);
    }
    else if(!strcasecmp(aux->nombre,"mount")){
       char nombre[16];
       limpiar_cadena(nombre,16);
       char path[100];
       limpiar_cadena(path,100);
       aux = aux->siguiente;
       while(aux!=NULL){
           if(!strcasecmp(aux->nombre, "path")){
               strcpy(path,aux->parametro);
           }else if(!strcasecmp(aux->nombre, "name")){
                strcpy(nombre,aux->parametro);
           }else{
               printf("mount no posee el parametro %s \n",aux->nombre);
           }
           aux = aux->siguiente;
       }
       MOUNT(path, nombre);
    }
    else if(!strcasecmp(aux->nombre,"unmount")){
            char id[4];
            limpiar_cadena(id,100);
            aux = aux->siguiente;
            while(aux!=NULL){
                if(!strcasecmp(aux->nombre, "id")){
                    strcpy(id,aux->parametro);
                }else{
                    printf("unmount no posee el parametro %s \n",aux->nombre);
                }
                aux = aux->siguiente;
            }
            UNMOUNT(id);
    }
    else if(!strcasecmp(aux->nombre,"rep")){
            char nombre[16];
            limpiar_cadena(nombre,16);
            char path[100];
            limpiar_cadena(path,100);
            char id[4];
            limpiar_cadena(id,4);
            aux = aux->siguiente;
            while(aux!=NULL){
                   if(!strcasecmp(aux->nombre, "path")){
                       strcpy(path,aux->parametro);
                   }else if(!strcasecmp(aux->nombre, "name")){
                        strcpy(nombre,aux->parametro);
                   }else if(!strcasecmp(aux->nombre, "id")){
                       strcpy(id,aux->parametro);
                  }else{
                       printf("rep no posee el parametro %s \n",aux->nombre);
                   }
                   aux = aux->siguiente;
            }
            REP(path,id,nombre);
        }else if(!strcasecmp(aux->nombre, "exec")){
            char path[100];
            limpiar_cadena(path,100);
            aux = aux->siguiente;
            while(aux!=NULL){
                   if(!strcasecmp(aux->nombre, "path")){
                       strcpy(path,aux->parametro);
                   }else{
                       printf("exec no posee el parametro %s \n",aux->nombre);
                   }
                   aux = aux->siguiente;
            }
            EXEC(path);
        }else if(!strcasecmp(aux->nombre, "MKFS")){
            char id[4];
            limpiar_cadena(id, 4);
            char type[5];
            limpiar_cadena(type, 5);
            aux = aux->siguiente;
            while(aux!=NULL){
                if(!strcasecmp(aux->nombre, "type")){
                    strcpy(type,aux->parametro);
                }
                else if(!strcasecmp(aux->nombre, "id")){
                    strcpy(id, aux->parametro);
                }else{
                    printf("MKFS no posee el parametro %s \n", aux->parametro);
                }
                aux = aux->siguiente;
            }
            MKFS(id, type);
        }else if(!strcasecmp(aux->nombre, "MKDIR")){
            char id[4];
            limpiar_cadena(id, 4);
            char path[100];
            limpiar_cadena(path, 100);
            int p = 0;
            aux = aux->siguiente;
            while(aux!=NULL){
                if(!strcasecmp(aux->nombre, "path")){
                    strcpy(path,aux->parametro);
                }
                else if(!strcasecmp(aux->nombre, "id")){
                    strcpy(id, aux->parametro);
                }else if(!strcasecmp(aux->nombre, "p")){
                    p = 1;
                }else{
                    printf("MKDIR no posee el parametro %s \n", aux->parametro);
                }
                aux = aux->siguiente;
            }
            MKDIR(id, path, p);
        }
}
int EJECUTAR_COMANDO(char *comando){
    int i = 0;
    int j = 0;
    //int terminado=1;
    strcat(comando," ");
    strcat(comando,"\n");
    Lista *comandos = (Lista*)malloc(sizeof(Lista));
    comandos->primero = NULL;
    char nombre[100];
    char parametro[100];
    limpiar_cadena(nombre, 100);
    limpiar_cadena(parametro, 100);
    int estado=0;
    int error=0;
    //basado en automata->
    while(comando[i]!='\0'&&error==0){
       if(estado==0){
           if((comando[i]>=65&&comando[i]<=90)||(comando[i]>=97&&comando[i]<=122)){
               nombre[j]=comando[i];
               estado = 1;
               j++;
           }else if(comando[i]=='#'){
               //printf("Comentario\n");
               estado = 9;
           }
           else if(comando[i]==' '||comando[i]=='\t'||comando[i]=='\n'){
               //printf("Error en el comando 0\n");
               //error=1;
           }else{
            error=10;
           }
       } else if(estado==1){
           if((comando[i]>=65&&comando[i]<=90)||(comando[i]>=97&&comando[i]<=122)){
               nombre[j]=comando[i];
               estado = 1;
               j++;
           }else if(comando[i]==' '||comando[i]=='\t'||comando[i]=='\n'){
               insertar_parametro(comandos,nombre,parametro);
               limpiar_cadena(nombre,100);
               limpiar_cadena(parametro,100);
               estado = 2;
               j=0;
           }else{
               //printf("Error en el comando,error 1\n");
               error=1;
           }
       }
       else if(estado==2){
            char x = comando[i];
           if(comando[i]=='-'||comando[i]=='–'||comando[i]=='+'){
               estado=3;
           }else if(comando[i]==' '||comando[i]=='\t'||comando[i]=='\n'){
               estado=2;
           }
           else if(comando[i]=='#'){
            estado = 9;
           }
           else{
               //printf("Error en el comando, error 2\n");
                 error=2;
           }
       }
       else if(estado==3){
           if((comando[i]>=65&&comando[i]<=90)||(comando[i]>=97&&comando[i]<=122)){
               nombre[j]=comando[i];
               estado = 4;
               j++;
           }else{
             //  printf("Error en el comando, error 3\n");
               error=3;
           }
       }
       else if(estado==4){
           if((comando[i]>=65&&comando[i]<=90)||(comando[i]>=97&&comando[i]<=122)){
               nombre[j]=comando[i];
               estado = 4;
               j++;
           }else if(comando[i]==':'){
               estado = 8;
           }else if(comando[i]==' '||comando[i]=='\t'){
               insertar_parametro(comandos,nombre,parametro);
               limpiar_cadena(nombre,100);
               limpiar_cadena(parametro,100);
               estado = 2;
               j=0;
           }
           else{
               //printf("Error en el comando, error 4\n");
               error=4;
           }
       }
       else if(estado==5){
           if(comando[i]=='"'){
               estado=6;
           }else if(comando[i]==' '||comando[i]=='\t'){
                estado = 5;
           }
           else{
               parametro[j]=comando[i];
               estado = 7;
               j++;
           }
       }
       else if(estado==6){
           if(comando[i]=='"'){
               insertar_parametro(comandos,nombre,parametro);
               limpiar_cadena(nombre,100);
               limpiar_cadena(parametro,100);
               estado = 2;
               j=0;
           }
           else{
               parametro[j]=comando[i];
               j++;
           }
       }
       else if(estado==7){
           if(comando[i]==' '||comando[i]=='\n'){
               insertar_parametro(comandos,nombre,parametro);
               limpiar_cadena(nombre,100);
               limpiar_cadena(parametro,100);
               estado = 2;
               j=0;
           }
           else{
               parametro[j]=comando[i];
               j++;
               if(comando[i+1]=='\0'){
                   insertar_parametro(comandos,nombre,parametro);
                   limpiar_cadena(nombre,100);
                   limpiar_cadena(parametro,100);
                   j=0;
                }
           }
       }
       else if(estado==8){
        if(comando[i]==':'){
            estado = 5;
            j=0;
        }else{
            error = 8;
        }
       }
       else if(estado==9){
        if(comando[i]!='\n'){
            //comentario
        }
       }
       i++;
    }
    //imprimir_lista(comandos);
    if(comandos->primero!=NULL){
        if(error == 0){
            ejecutar_lista(comandos);
        }else{
            printf("Error estado %d\n", error);
        }
    }else{
        printf("sin comando \n");
//home/javierb/github/Proyecto1_201213619/201213619
    }
    return 0;
}

void insertar_parametro(Lista *lista, char *nombre, char*parametro){
    if(lista->primero==NULL){
        NodoLista* nuevo = (NodoLista*)malloc(sizeof(NodoLista));
        strcpy(nuevo->nombre, nombre);
        strcpy(nuevo->parametro, parametro);
        nuevo->siguiente = NULL;
        lista->primero = nuevo;
    }else{
        NodoLista* nuevo = (NodoLista*)malloc(sizeof(NodoLista));
        strcpy(nuevo->nombre, nombre);
        strcpy(nuevo->parametro, parametro);
        NodoLista* ultimo = lista->primero;
        while(ultimo->siguiente!=NULL){
            ultimo = ultimo->siguiente;
        }
        ultimo->siguiente = nuevo;
        nuevo->siguiente = NULL;
    }
}


void imprimir_lista(Lista *lista){
    NodoLista *nodo = lista->primero;
    while(nodo!=NULL){
        printf("(%s|%s)->",nodo->nombre, nodo->parametro);
        nodo = nodo->siguiente;
    }
    printf("null\n");
}/*

    FILE* script = fopen(archivo,"r");
    char linea;
    rewind(script);
    while(!feof(script)){
        fscanf(script,linea);

        limpiar_cadena(linea, 250);
    }
    while (!feof(script)) {
        const char *line = readLine(script);
        EJECUTAR_COMANDO(linea);
        printf("%s\n", line);

}*/

void EXEC(char *archivo){
    FILE *fp;
        char *line = NULL;
        size_t len = 0;
        ssize_t read;

       fp = fopen(archivo,"r");
        if (fp == NULL)
            exit(EXIT_FAILURE);

       while ((read = getline(&line, &len, fp)) != -1) {
            //printf("Retrieved line of length %zu :\n", read);
            //printf("%s", line);
            EJECUTAR_COMANDO(line);
        }

       free(line);
       // exit(EXIT_SUCCESS);
    }
