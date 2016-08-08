#ifndef ADMIN_DISCOS_H_INCLUDED
#define ADMIN_DISCOS_H_INCLUDED
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include <lwh.h>
#define NOPARTICIONES 4

typedef struct EXTENDED_BOOT_RECORD{
    char part_status;
    char part_fit;
    int part_start;
    int part_size;
    int part_next;
    char part_name[16];
}EBR;

typedef struct  PARTITION{
    char part_status;//Activa, Desactiva
    char part_type;//Primara, Extendida
    char part_fit;//Best, First, Worst
    int part_start;//
    int part_size;
    char part_name[16];
}Partition;

typedef struct MASTERBOOTRECORD{
    int mbr_tam;
    char mbr_fecha_creacion[19];
    int mbr_disk_signature;
    Partition mbr_partition[NOPARTICIONES];
}MBR;

typedef struct MountedParts{
    char name[4];
    char nombre_part[16];
    int bit_start;
    char type;
    struct MountedPartition* siguiente;
}MountedPartition;

typedef struct MountedDisk{
    char path[200];
    char letter;
    char count;
    MountedPartition* first;
    struct MountedDisk* siguiente;
}MountedDisk;

typedef struct Mounted{
    char current_letter;
    MountedDisk* first;
}Mounted;


char* timeToChar();
char *carpeta(char *path);
void MKDISK(int  size,int unit, char* path);
void RMDISK(char* path);
void FDISK(int size, char unit, char* path, char type, char fit, char del, int add, char* name);
void UNMOUNT(char *name);
void MOUNT(char *path, char *name);
void MKFS(char *id, char *type);
void MKDIR(char *id, char *path, int p);
int partition_disponible(MBR *mbr);
int primer_ajuste(int required, FILE *file);
void Reescribir_mbr(MBR mbr, FILE *disco);
MBR leerMBR(FILE* disco);
int siguiente_particion(Partition** vector, int index, int tam);
int extendida_creada(MBR *mbr);
int indice_extendida(MBR *mbr);
Partition buscar_particion_nombre(char *disc, char *name);
MountedDisk *b_disco_montado(char *path);
void REP(char* path, char *id, char *name);
void rep_mbr(char* id, char *path);
void rep_disk(char* id, char *path);
void rep_avd(char* id, char *path);
void rep_dd(char* id, char *path);
void rep_inode(char* id, char *path);
void rep_block(char* id, char *path);
void rep_tree(char* id, char *path);
void rep_sb(char* id, char *path);
void rep_avds(char* id, char *path);
void rep_Logs(char* id, char *path);

void imprimirParticion(Partition *part, FILE *file,FILE* disco);
EBR buscar_logica_nombre(char *disc, char *name);
void Montar_Particion(MountedDisk *disc, int start_bit, char tipo, char* name);
#endif
