#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{

    char *buffer = NULL;
    int read;
    unsigned int len;
    while(1)
    {
        printf("Ingrese un comando...\n");
        read = getline(&buffer, &len, stdin);
        int posx= strlen(buffer)-1;
        buffer[posx]=='\0';
        if (-1 != read)
        {
            int salir = 0;
            while(!salir){
                int pos= strlen(buffer)-1;
                if(buffer[pos]=='\\'){
                    char *buffer2 = NULL;
                    unsigned int len2;
                    read = getline(&buffer2, &len2, stdin);
                    if (-1 != read){
                        strcat(buffer,buffer2);
                    }
                    free(buffer2);
                }else{
                    salir = 1;
                }
            }
            int len = strlen(buffer);
            buffer[len-1]=' ';
            EJECUTAR_COMANDO(buffer);
        }
        else{
            printf("No line read...\n");
        }
        limpiar_cadena(buffer,strlen(buffer));
        //free(buffer);
    }
    return 0;
}
