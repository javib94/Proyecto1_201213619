#ifndef EXT2_H_INCLUDED
#define EXT2_H_INCLUDED
#include <time.h>
#include <stdio.h>
#include <string.h>
typedef struct Superbloque{
    int sb_arbol_virtual_count;
    int sb_detalle_directorio_count;
    int sb_inodes_count;
    int sb_blocks_count;
    int sb_free_arbol_virtual_count;
    int sb_free_detalle_directorio_count;
    int sb_free_blocks_count;
    int sb_free_inodes_count;
    time_t sb_mtime;
    time_t sb_umtime;
    int sb_mnt_count;
    int sb_magic;
    int sb_avd_size;
    int sb_detalle_directorio_size;
    int sb_inode_size;
    int sb_block_size;
    int sb_ap_avd;
    int sb_ap_bitmap_avd;
    int sb_ap_detalle_directorio;
    int sb_ap_bitmap_detalle_directorio;
    int sb_ap_tabla_inodo;
    int sb_ap_bitmap_tabla_inodo;
    int sb_ap_bloque;
    int sb_ap_bitmap_bloque;
    int sb_ap_log;
    int sb_ap_copia_super_bloque;
    int sb_first_free_bit_avd;
    int sb_first_free_bit_detalle_directorio;
    int sb_first_free_bit_tabla_inodo;
    int sb_first_free_bit_bloques;
}Superbloque;

typedef struct avd{
    time_t avd_fecha_creacion;
    char avd_name_directorio[16];
    int avd_ap_array_subdirectorios[6];
    int avd_ap_detalle_directorio;
    int avd_ap_avd;
}avd;

typedef struct Archivo{
    char dd_file_nombre[16];
    int dd_file_ap_inodo;
    time_t dd_file_date_creacion;
    time_t dd_file_date_modificacion;
}Archivo;

typedef struct D_Directorio{
    Archivo dd_Archivo[5];
    int dd_ap_detalle_directorio;
}D_Directorio;


typedef struct Inodo{
    int i_count_inodo;
    int i_size_file;
    int i_count_bloques_asignados;
    int i_array_bloques[4];
    int i_ap_indirecto;
}Inodo;

typedef struct Data_Block{
    char db_data[25];
}Data_Block;

typedef struct Log{
    int log_tipo_operacion;
    int log_tipo;
    char log_nombre[16];
    char log_contenido;
    time_t log_fecha;
}Log;

Superbloque new_Superbloque(int bit_start_partition, int size);
avd new_avd(char *name);
Archivo* new_archivo(char* nombre_archivo);
time_t HoraActual();
Superbloque leer_SB(FILE* disco, int byte_start);
avd leer_avd(FILE* disco, int byte_start);
void crear_raiz(FILE* disco, int part_start);
void crear_carpeta(FILE *disco, int byte_start, char *path);
void limpiar_cadena(char* cadena, int tam);
int existe_carpeta(FILE *disco, int byte_start, char *path);
int existe_carpeta_en_nodo(FILE *disco, avd *nodo, char *nombre_carpeta);
void actualizar_sb(FILE *disco, int byte_start, Superbloque *sb);
int buscar_carpeta(FILE *disco, char* path, int byte_start);
int existe_padre(FILE *disco, int bit_start, char *path);
















#endif
