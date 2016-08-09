#include "ext2.h"

Superbloque new_Superbloque(int bit_start_partition, int size){
    Superbloque sp;
    int N = (size-256)/391;
    sp.sb_arbol_virtual_count=N;
    sp.sb_detalle_directorio_count=N;
    sp.sb_inodes_count=N;
    sp.sb_blocks_count=N;
    sp.sb_free_blocks_count=N;
    sp.sb_free_inodes_count=N;
    sp.sb_mnt_count=0;
    sp.sb_magic=201213619;
    sp.sb_mtime = HoraActual();
    sp.sb_umtime = HoraActual();
    sp.sb_avd_size=sizeof(avd);
    sp.sb_detalle_directorio_size=sizeof(D_Directorio);
    sp.sb_inode_size=sizeof(Inodo);
    sp.sb_block_size=sizeof(Data_Block);
    sp.sb_ap_avd = bit_start_partition+sizeof(Superbloque)+N;
    sp.sb_ap_bitmap_avd = bit_start_partition+sizeof(Superbloque);
    sp.sb_ap_detalle_directorio=bit_start_partition+sizeof(Superbloque)+N+N*sizeof(avd)+N;
    sp.sb_ap_bitmap_detalle_directorio=bit_start_partition+sizeof(Superbloque)+N+N*sizeof(avd);
    sp.sb_ap_tabla_inodo = sp.sb_ap_detalle_directorio+ N* sizeof(D_Directorio)+N;
    sp.sb_ap_bitmap_tabla_inodo= sp.sb_ap_detalle_directorio+ N* sizeof(D_Directorio);
    sp.sb_ap_bloque = sp.sb_ap_tabla_inodo+N*sizeof(Inodo)+N;
    sp.sb_ap_bitmap_bloque = sp.sb_ap_tabla_inodo+N*sizeof(Inodo);
    sp.sb_ap_log = sp.sb_ap_bloque + 2*N+ 2*N*sizeof(Data_Block);
    sp.sb_ap_copia_super_bloque = sp.sb_ap_log+N*sizeof(Log);
    sp.sb_first_free_bit_avd = sp.sb_ap_bitmap_avd;
    sp.sb_first_free_bit_detalle_directorio = sp.sb_ap_bitmap_detalle_directorio;
    sp.sb_first_free_bit_tabla_inodo = sp.sb_ap_bitmap_tabla_inodo;
    sp.sb_first_free_bit_bloques=sp.sb_ap_bitmap_bloque;
    return sp;
}
avd new_avd(char *name){
    avd nuevo;
    int i=0;
    for (i=0;i<6;i++){
        nuevo.avd_ap_array_subdirectorios[i] = -1;
    }
    nuevo.avd_ap_avd = -1;
    nuevo.avd_fecha_creacion = HoraActual();
    strcpy(nuevo.avd_name_directorio, name);
    return nuevo;
}
time_t HoraActual(){
    time_t tiempo = time(0);
    struct tm *tlocal = localtime(&tiempo);
    return tiempo;
}
Superbloque leer_SB(FILE* disco, int byte_start){
    Superbloque sb;
    fseek(disco,byte_start, SEEK_SET);
    fread(&sb, sizeof(Superbloque), 1, disco);
    return sb;
}
void crear_raiz(FILE *disco, int part_start){
    Superbloque sb = leer_SB(disco,part_start);
    if(sb.sb_magic==201213619){
        int n = sb.sb_avd_size;
        char bitmap[n];
        int bit_arbol = sb.sb_ap_avd;
        fseek(disco, part_start+sizeof(Superbloque), SEEK_SET);
        fread(bitmap, sizeof(char), n, disco);
        avd carpeta;
        carpeta.avd_name_directorio[0]="/";
        carpeta.avd_fecha_creacion = HoraActual();
        carpeta.avd_ap_avd = -1;
        int i = 0;
        for(i=0;i<5;i++){
            carpeta.avd_ap_array_subdirectorios[i] = -1;
        }
        carpeta.avd_ap_detalle_directorio = -1;
        fseek(disco, sb.sb_ap_avd, SEEK_SET);
        fwrite(&carpeta, sizeof(avd), 1, disco);
        fseek(disco, sb.sb_ap_bitmap_avd, SEEK_SET);
        fwrite(bitmap, sizeof(char), n, disco);
        sb.sb_first_free_bit_avd++;
        sb.sb_free_arbol_virtual_count--;
        actualizar_sb(disco, part_start, &sb);
    }else{
        printf("error de super bloque");
    }
}

void crear_carpeta(FILE *disco, int byte_start, char *path){
    Superbloque sb = leer_SB(disco, byte_start);
    if(sb.sb_magic==201213619){
        if(!existe_carpeta(disco, byte_start,path)){
            char aux[100];
            int tam = strlen(path);
            int i = 0;
            int puntero = sb.sb_ap_avd;
            avd carpeta_actual= leer_avd(disco, sb.sb_ap_avd);
            while(i<tam){
                limpiar_cadena(aux, 100);
                int j = 0;
                while(path[i]!='/'&&path[i]!='\0'){
                    aux[j] = path[i];
                    i++;
                    j++;
                }
                int existe = 6;
                while(carpeta_actual.avd_ap_avd!=-1&&existe>5){
                    puntero = carpeta_actual.avd_ap_avd;
                    existe = existe_carpeta_en_nodo(disco, &carpeta_actual, aux);
                    carpeta_actual = leer_avd(disco, carpeta_actual.avd_ap_avd);
                }
                if(existe<6){
                    carpeta_actual = leer_avd(disco, carpeta_actual.avd_ap_array_subdirectorios[existe]);
                }else{
                    //creando carpeta nueva
                    int libre = 6;
                    int k = 0;
                    while(k<6){
                        if(carpeta_actual.avd_ap_array_subdirectorios[k]==-1){
                            libre = k;
                        }
                        k++;
                    }
                    if(libre<6){//si hay un sub libre
                        avd nueva_carpeta = new_avd(aux);
                        int n = sb.sb_first_free_bit_avd-sb.sb_ap_bitmap_avd;
                        int posicion = sb.sb_ap_avd + n*sizeof(avd);
                        carpeta_actual.avd_ap_array_subdirectorios[libre]=posicion;
                        fseek(disco, puntero, SEEK_SET);
                        fwrite(&carpeta_actual, sizeof(avd), 1, disco);
                        fseek(disco, posicion, SEEK_SET);
                        fwrite(&nueva_carpeta, sizeof(avd), 1, disco);
                        sb.sb_first_free_bit_avd++;
                        sb.sb_free_arbol_virtual_count--;
                        fseek(disco, sb.sb_first_free_bit_avd, SEEK_SET);
                        fwrite('1', sizeof(char), 1, disco);
                        actualizar_sb(disco, byte_start, &sb);
                    }else{//es necesario crear hermano
                        avd hermano = new_avd(carpeta_actual.avd_name_directorio);
                        int n = sb.sb_first_free_bit_avd-sb.sb_ap_bitmap_avd;
                        int posicion = sb.sb_ap_avd + n*sizeof(avd);
                        carpeta_actual.avd_ap_avd = posicion;
                        fseek(disco, puntero, SEEK_SET);
                        fwrite(&carpeta_actual, sizeof(avd), 1, disco);
                        fseek(disco, posicion, SEEK_SET);
                        fwrite(&hermano, sizeof(avd), 1, disco);
                        sb.sb_first_free_bit_avd++;
                        sb.sb_free_blocks_count--;
                        fseek(disco, sb.sb_first_free_bit_avd, SEEK_SET);
                        fwrite('1', sizeof(char), 1, disco);
                        actualizar_sb(disco, byte_start, &sb);
                        puntero = posicion;
                        //hermano creado
                        avd nueva_carpeta = new_avd(aux);
                        n = sb.sb_first_free_bit_avd-sb.sb_ap_bitmap_avd;
                        posicion = sb.sb_ap_avd + n*sizeof(avd);
                        hermano.avd_ap_array_subdirectorios[0] = posicion;
                        fseek(disco, puntero, SEEK_SET);
                        fwrite(&hermano, sizeof(avd), 1, disco);
                        fseek(disco, posicion, SEEK_SET);
                        fwrite(&nueva_carpeta, sizeof(avd), 1, disco);
                        sb.sb_first_free_bit_avd++;
                        sb.sb_free_blocks_count--;
                        fseek(disco, sb.sb_first_free_bit_avd, SEEK_SET);
                        fwrite('1', sizeof(char), 1, disco);
                        actualizar_sb(disco, byte_start, &sb);
                    }
                }
                i++;
            }
        }
    }else{
        printf("error de superbloque");
    }
}

void actualizar_sb(FILE *disco, int byte_start, Superbloque *sb){
    fseek(disco, byte_start, SEEK_SET);
    fwrite(sb, sizeof(Superbloque), 1, disco);
    int pos_copia = sb->sb_ap_copia_super_bloque;
    fseek(disco, pos_copia, SEEK_SET);
    fwrite(sb, sizeof(Superbloque), 1, disco);
}

int existe_carpeta(FILE *disco, int byte_start, char *path){
    Superbloque sb = leer_SB(disco, byte_start);
    fseek(disco, sb.sb_ap_avd, SEEK_SET);
    char aux[100];
    int tam = strlen(path);
    int i = 0;
    avd carpeta_actual= leer_avd(disco, sb.sb_ap_avd);
    while(i<tam){
        limpiar_cadena(aux, 100);
        int j = 0;
        while(path[i]!='/'&&path[i]!='\0'){
            aux[j] = path[i];
            i++;
            j++;
        }
        int existe = 6;
        while(carpeta_actual.avd_ap_avd!=-1&&existe>5){
            existe = existe_carpeta_en_nodo(disco, &carpeta_actual, aux);
            carpeta_actual = leer_avd(disco, carpeta_actual.avd_ap_avd);
        }

        if(existe<6){
            carpeta_actual = leer_avd(disco, carpeta_actual.avd_ap_array_subdirectorios[existe]);
        }else{
            i = tam;
        }
        i++;
    }
   if(i>=tam){
       return 1;
   }
   return 0;
}
int existe_carpeta_en_nodo(FILE *disco, avd *nodo, char *nombre_carpeta){
    int i = 0;
    while(i<6){
        if(nodo->avd_ap_array_subdirectorios[i]!=-1){
            avd subdirectorio = leer_avd(disco, nodo->avd_ap_array_subdirectorios[i]);
            if(!strcasecmp(nombre_carpeta, subdirectorio.avd_name_directorio)){
               return i;
            }
        }
    }
    return 6;// out of bounds
}
void limpiar_cadena(char* cadena, int tam){
    while(tam>=0){
        cadena[tam]= '\0';
        tam--;
    }
}

avd leer_avd(FILE *disco, int pos){
    avd carpeta;
    fseek(disco, pos, SEEK_SET);
    fread(&carpeta, sizeof(avd),1,disco);
    return carpeta;
}

int existe_padre(FILE *disco, int bit_start, char *path){
   char padre[100];
   strcpy(padre, path);
   int i = strlen(padre);
   while(padre[i]!='/'){
       i--;
       padre[i]= '\0';
   }
   return existe_carpeta(disco, bit_start, padre);
}
/*
Inodo *newInodo( int uid, int gid, int size, time_t* atime, time_t* ctime, time_t* mtime,
    int block[], char type, int perm){
    Inodo* nuevo = (Inodo*)malloc(sizeof(Inodo));
    nuevo->i_uid = uid;
    nuevo->i_gid = gid;
    nuevo->i_size = size;
    nuevo->i_atime = atime;
    nuevo->i_ctime = ctime;
    nuevo->i_mtime = mtime;
    int i = 0;
    for(i=0;i<15;i++){
        nuevo->i_block[i]= block[i];
    }
    nuevo->i_type = type;
    nuevo->i_perm = perm;
    return nuevo;
}

Inodo* leerInodo(FILE* archivo, int byteInicio){
    Inodo* inodo;
    fseek(archivo,byteInicio,SEEK_SET);
    fread(inodo, sizeof(Inodo), 1, archivo);
    return inodo;
}

Bloque_Carpeta* leerBloque_Carpeta(FILE* archivo, int byteInicio){
    Bloque_Carpeta* bloque;
    fseek(archivo,byteInicio,SEEK_SET);
    fread(bloque, sizeof(Bloque_Carpeta), 1, archivo);
    return bloque;
}

Bloque_Archivo* leerBloque_Archivo(FILE* archivo, int byteInicio){
    Bloque_Archivo* bloque;
    fseek(archivo,byteInicio,SEEK_SET);
    fread(bloque, sizeof(Bloque_Archivo), 1, archivo);
    return bloque;
}

Bloque_Apuntadores* leerBloque_Apuntadores(FILE* archivo, int byteInicio){
    Bloque_Apuntadores* bloque;
    fseek(archivo,byteInicio,SEEK_SET);
    fread(bloque, sizeof(Bloque_Apuntadores), 1, archivo);
    return bloque;
}

void escribirInodo(FILE* archivo, int byteInicio,Inodo *inodo){
     fseek(archivo,byteInicio,SEEK_SET);
     fwrite(inodo, sizeof(Inodo), 1, archivo);
}

void escribirBloque_Carpeta(FILE* archivo, int byteInicio,Bloque_Carpeta* bloque){
    fseek(archivo,byteInicio,SEEK_SET);
    fwrite(bloque, sizeof(Bloque_Carpeta), 1, archivo);
}

void escribirBloque_Archivo(FILE* archivo, int byteInicio, Bloque_Archivo* bloque){
    fseek(archivo,byteInicio,SEEK_SET);
    fwrite(bloque, sizeof(Bloque_Archivo), 1, archivo);
}

void escribirBloque_Apuntadores(FILE* archivo, int byteInicio,Bloque_Apuntadores *bloque){
    fseek(archivo,byteInicio,SEEK_SET);
    fwrite(bloque, sizeof(Bloque_Apuntadores), 1, archivo);
}

int calcular_inodos(int bytes_disponibles){
    int n=(bytes_disponibles-sizeof(Superbloque))/(1+3+sizeof(Inodo)+3*sizeof(Bloque_Archivo));
}
*/
