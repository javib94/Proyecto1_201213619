#include "admin_discos.h"

Mounted montados;

char* timeToChar(){
        char* t = malloc(19);
        time_t tiempo = time(0);
        struct tm *tlocal = localtime(&tiempo);
        strftime(t, 19, "%d/%m/%y--%H:%M:%S", tlocal);
        //printf("%s",t);
        return t;
}




char *carpeta(char *path){
    int tam = strlen(path);
    char *nueva;
    strcpy(nueva, path);
    char caracter  = '\0';
    while(caracter !='/'){
        nueva[tam] = '\0';
        tam--;
        caracter = nueva[tam];
    }
    return nueva;
}
void MKDISK(int  size,int unit, char* path){//SIZE EN BYTES
    MBR mbr;
    if(size>0){
        if(unit=='k'||unit=='K'){
            size*=1024;
        }else if(unit=='M'||unit=='m'){
            size*=1024*1024;
        }
        mbr.mbr_tam = size;
        strcpy(mbr.mbr_fecha_creacion,timeToChar());
        srand(time(NULL));
        mbr.mbr_disk_signature = rand();
        int i = 0;
        for(i= 0; i<NOPARTICIONES;i++){
           mbr.mbr_partition[i].part_fit=0;
           strcpy(mbr.mbr_partition[i].part_name , "");
           mbr.mbr_partition[i].part_size= 0;
           mbr.mbr_partition[i].part_start=0;
           mbr.mbr_partition[i].part_status= 'D';//ENABLE OR DISABLE
           mbr.mbr_partition[i].part_type=0;
        }
        char comando[200];
        limpiar_cadena(comando,200);
        char cp[100];
        limpiar_cadena(cp,100);
        strcpy(cp,carpeta(path));
        sprintf(comando,"mkdir \"%s\" -p",cp);
        system(comando);
        FILE *archivo = fopen(path, "w");
        fclose(archivo);
        archivo = fopen(path, "rb+");
        rewind(archivo);
        fwrite(&mbr, sizeof(MBR), 1, archivo);
        size = size-sizeof(MBR);
        while(size>0){
            fputc('\0',archivo);
            size--;
        }
        fclose(archivo);
        printf("disco creado..\n");
    }else{
        printf("tamaño menor a cero");
    }
}
void RMDISK(char* path){
    printf("¿Está seguro que desea eliminar el disco?s/n... ");
    char resultado;
    scanf("%c",&resultado);
    if(resultado=='s'){
       int eliminado= remove(path);
       if(eliminado==0){
           printf("disco removido con exito\n");
       }else{
           printf("Disco no encotrado");
       }
    }else{
        printf("Eliminacion cancelada\n");
    }
}
void Montar_Particion(MountedDisk *disc, int start_bit, char tipo, char *nombre){
    if(disc->first==NULL){
        disc->first = (MountedPartition*)malloc(sizeof(MountedPartition));
        disc->first->bit_start = start_bit;
        limpiar_cadena(disc->first->name,4);
        disc->first->name[0]='v';
        disc->first->name[1]='d';
        disc->first->name[2]=disc->letter;
        disc->first->name[3]=disc->count;
        disc->first->name[4]='\0';
        disc->first->type= tipo;
        strcpy(disc->first->nombre_part, nombre);
        disc->first->siguiente = NULL;
        disc->count++;
        printf("Montado como disco %s\n",disc->first->name);
    }else{
        MountedPartition* nuevo = (MountedPartition*)malloc(sizeof(MountedPartition));
        nuevo->bit_start = start_bit;
        nuevo->name[0]='v';
        nuevo->name[1]='d';
        nuevo->name[2]=disc->letter;
        nuevo->name[3]=disc->count;
        nuevo->name[4]='\0';
        nuevo->type = tipo;
        strcpy(nuevo->nombre_part, nombre);
        nuevo->siguiente = NULL;
        disc->count++;
        nuevo->siguiente = disc->first;
        disc->first = nuevo;
        printf("Montado como disco %s\n",disc->first->name);
    }
}
void UNMOUNT(char *name){
    char letra = name[2];
    char num = name[3];
    MountedDisk *disc = montados.first;
    while(disc!=NULL&&disc->letter!=letra){
        disc=disc->siguiente;
    }
    if(disc!=NULL){
        MountedPartition* part = disc->first;
        MountedPartition* ant = disc->first;
        while(part!=NULL&&part->name[3]!=num){
            ant = part;
            part = part->siguiente;
        }
        if(part!=NULL){
            ant->siguiente = part->siguiente;
            FILE *disco;
            disco = fopen(disc->path,"rb+");
            Superbloque sb = leer_SB(disco, part->bit_start);
            sb.sb_umtime = HoraActual();
            actualizar_sb(disco, part->bit_start, &sb);
            fclose(disco);
            free(part);
        }else{
            printf("Particion no encontrada\n");
        }

    }else{
        printf("Disco no encontrado \n");
    }
}

void MOUNT(char *path, char *name){
    Partition part = buscar_particion_nombre(path,name);
    if(part.part_start!=0){
        if(montados.first==NULL){
            montados.current_letter='a';
            montados.first = (MountedDisk*)malloc(sizeof(MountedDisk));
            strcpy(montados.first->path,path);
            montados.first->count='1';
            montados.first->letter = montados.current_letter;
            montados.first->first = NULL;
            Montar_Particion(montados.first,part.part_start,part.part_type, name);
            montados.current_letter++;
        }else{
            MountedDisk* actual = b_disco_montado(path);
            if(actual==-1){
                MountedDisk* nuevo = (MountedDisk*)malloc(sizeof(MountedDisk));
                strcpy(nuevo->path,path);
                nuevo->count='1';
                nuevo->letter = montados.current_letter;
                nuevo->first = NULL;
                montados.current_letter++;
                nuevo->siguiente = NULL;
                montados.first = nuevo;
                Montar_Particion(nuevo,part.part_start,part.part_type, name);
            }else{
                Montar_Particion(actual,part.part_start, part.part_type,name);
            }
        }
        FILE *disco;
        disco = fopen(path,"rb+");
        Superbloque sb = leer_SB(disco, part.part_start);
        sb.sb_mtime = HoraActual();
        sb.sb_mnt_count++;
        actualizar_sb(disco, part.part_start, &sb);
        fclose(disco);
    }else{

        printf("particion no encontrada \n");
   }
}

Partition buscar_particion_nombre(char *disc, char *name){
    FILE *disco;
    Partition part;
    disco = fopen(disc,"rb+");
    if(disco!=NULL){
        MBR mbr;
        fread(&mbr, sizeof(MBR),1,disco);
        int i;
        for(i = 0;i<NOPARTICIONES;i++){
            if(!strcasecmp(mbr.mbr_partition[i].part_name,name)){
                fclose(disco);
                return mbr.mbr_partition[i];
            }
        }
        EBR ebr = buscar_logica_nombre(disc, name);
        if(ebr.part_size>0){
            part.part_fit = ebr.part_fit;
            strcpy(part.part_name,ebr.part_name);
            part.part_size= ebr.part_size;
            part.part_start= ebr.part_start;
            part.part_status = ebr.part_status;
            part.part_type = 'L';
        }
        fclose(disco);
        return part;
    }else{
        printf("Ese disco no existe\n");
        return part;
    }
}



void FDISK(int size,char unit, char *path, char type, char fit, char del, int add, char *name){
    FILE *disco=fopen(path, "rb+");
    if(disco!=NULL){
        if(size>0){
            if(unit=='b'||unit=='B'){
                size = size*1;
            }
            else if(unit=='k'||unit=='K'){
                size = size*1024;
            }else if(unit=='m'||unit=='M'){
                size = size*1024;
            }
        }
        rewind(disco);
        MBR mbr;
        fread(&mbr, sizeof(MBR), 1, disco);//Leyendo mbr del disco
        if(del!='F'){//si del ya no es f
            int x = 0;
            int i = 5;
            while(x<4){
                if(!strcmp(name,&mbr.mbr_partition[x].part_name)){
                    i=x;
                }
                x++;
            }
            if(i>4){
                printf("La particion no existe");
            }else{
                int tam =  mbr.mbr_partition[i].part_size;
                fseek(disco, mbr.mbr_partition[i].part_start, SEEK_SET);
                while(tam>0){
                    char c = '\0';
                    fwrite(&c, 1,1,disco);
                    tam--;
                }
                mbr.mbr_partition[i].part_start =0;
                mbr.mbr_partition[i].part_fit =0;
                mbr.mbr_partition[i].part_status ='D';
                mbr.mbr_partition[i].part_size =0;
                mbr.mbr_partition[i].part_type =0;
                strcpy(mbr.mbr_partition[i].part_name, "");
                Reescribir_mbr(mbr,disco);
                printf("Particion Borrada\n");
            }
        }else if(add!=0){
            int x = 0;
            int i = 5;
            while(x<4){
                if(!strcmp(name,&mbr.mbr_partition[x].part_name)){
                    i=x;
                }
                x++;
            }
            if(i>4){
                printf("La particion no existe");
            }else{
                int tam =  mbr.mbr_partition[i].part_size;
                if(add<0){
                    int index = mbr.mbr_partition[i].part_start+mbr.mbr_partition[i].part_size-1;
                    add=-add;
                    int limite= mbr.mbr_partition[i].part_start+mbr.mbr_partition[i].part_size-1-add;
                    while(index>limite){
                        fseek(disco, index,SEEK_SET);
                        char c='\0';
                        fwrite(&c, 1,1,disco);
                        index--;
                    }
                    mbr.mbr_partition[i].part_size=mbr.mbr_partition[i].part_size-add;
                    Reescribir_mbr(mbr,disco);
                    printf("La partición redujo su tamaño\n");
                }else{
                    int index = mbr.mbr_partition[i].part_start;
                    index+= mbr.mbr_partition[i].part_size-1;
                    int libre = 1;
                    while(libre&&index<mbr.mbr_partition[i].part_size-1+add){
                        fseek(disco, index,SEEK_SET);
                        char c;
                        fread(&c, 1,1,disco);
                        if(c!='\0'){
                            libre=0;
                        }
                        index++;
                    }
                    if(libre){
                        index = mbr.mbr_partition[i].part_start+mbr.mbr_partition[i].part_size-1;
                        while(index<mbr.mbr_partition[i].part_start+mbr.mbr_partition[i].part_size+add-1){
                            fseek(disco, index,SEEK_SET);
                            char c='x';
                            fwrite(&c, 1,1,disco);
                        }
                        mbr.mbr_partition[i].part_size+=add;
                        Reescribir_mbr(mbr,disco);
                        printf("La partición aumento su tamaño\n");
                    }else{
                        printf("No hay espacio suficiente\n");
                    }
                }
                Reescribir_mbr(mbr,disco);
            }
        }else{
            if(type=='p'||type=='P'){
                int posicion = partition_disponible(&mbr);
                if(posicion!=-1){
                    //buscar espacio disponible
                    int byte_start_new = primer_ajuste(size,disco);// first_valid_position(size, disco);
                    if(byte_start_new!=-1){
                        mbr.mbr_partition[posicion].part_fit=fit;
                        mbr.mbr_partition[posicion].part_size=size;
                        mbr.mbr_partition[posicion].part_start=byte_start_new;
                        mbr.mbr_partition[posicion].part_type=type;
                        mbr.mbr_partition[posicion].part_status='E';
                        strcpy(mbr.mbr_partition[posicion].part_name, name);
                        Reescribir_mbr(mbr, disco);
                        int tam = size;
                        fseek(disco,byte_start_new,SEEK_SET);
                        while(tam>0){
                            char c = 'x';
                            fwrite(&c, 1,1,disco);
                            tam--;
                        }
                        printf("Particion creada satisfactoriamente: %s \n ",name);
                    }else{
                        printf("Espacio insuficiente para crear la particion\n");
                    }
                }else{
                    printf("Este disco no puede tener mas particiones\n");
                }
            }else if(type=='e'||type=='E'){
                if(extendida_creada(&mbr)){
                    printf("No se puede crear mas de una particion extendida\n");
                }else {
                    int posicion = partition_disponible(&mbr);
                    if(posicion!=-1){
                        //buscar espacio disponible
                        int byte_start_new = primer_ajuste(size, disco);
                        if(byte_start_new!=-1){
                            mbr.mbr_partition[posicion].part_fit=fit;
                            mbr.mbr_partition[posicion].part_size=size;
                            mbr.mbr_partition[posicion].part_start=byte_start_new;
                            mbr.mbr_partition[posicion].part_type=type;
                            mbr.mbr_partition[posicion].part_status='E';
                            strcpy(mbr.mbr_partition[posicion].part_name, name);
                            Reescribir_mbr(mbr, disco);
                            int tam = size;
                            fseek(disco,byte_start_new,SEEK_SET);
                            while(tam>0){
                                char c = 'y';
                                fwrite(&c, 1,1,disco);
                                tam--;
                            }
                            EBR ebr;
                            ebr.part_status='D';
                            ebr.part_fit='W';
                            strcpy(ebr.part_name,"");
                            ebr.part_size=0;
                            ebr.part_start=byte_start_new;
                            ebr.part_next=-1;
                            fseek(disco, byte_start_new,SEEK_SET);
                            fwrite(&ebr,sizeof(EBR),1,disco);
                            printf("Particion Extendida Creada: %s \n",name);
                        }else{
                            printf("Espacio insuficiente para crear la particion\n");
                        }
                    }else{
                        printf("Este disco no puede tener mas particiones\n");
                    }
                }
            }else if(type='l'||type=='L'){
                if(extendida_creada(&mbr)){
                    int indice  = indice_extendida(&mbr);
                    if(mbr.mbr_partition[indice].part_size>size){
                        int byte_start = mbr.mbr_partition[indice].part_start;
                        EBR ebr;//SEGUIR AQUI CON HACER PARTICIONES LOGICAS
                        fseek(disco,byte_start,SEEK_SET);
                        fread(&ebr, sizeof(EBR), 1, disco);
                        if(ebr.part_size>0){
                            while(ebr.part_next!=-1){
                                byte_start=ebr.part_next;
                                fseek(disco,byte_start,SEEK_SET);
                                fread(&ebr, sizeof(EBR), 1, disco);
                            }
                            int inicio = byte_start+ebr.part_size;
                            EBR ebr2;//Nuevo ebr
                            int byte_final = inicio+size-1;
                            if(byte_final<=mbr.mbr_partition[indice].part_start+mbr.mbr_partition[indice].part_size-1){
                                strcpy(ebr2.part_name,name);
                                ebr2.part_fit = fit;
                                ebr2.part_size=size;
                                ebr2.part_status= 'E';
                                ebr2.part_next = -1;
                                ebr2.part_start= inicio;
                                fseek(disco,inicio,SEEK_SET);
                                fwrite(&ebr2, sizeof(EBR), 1, disco);
                                int tam = size-sizeof(EBR);
                                fseek(disco,inicio+sizeof(EBR),SEEK_SET);
                                while(tam>0){
                                    char c = 'z';
                                    fwrite(&c, 1,1,disco);
                                    tam--;
                                }
                                ebr.part_next= inicio;
                                fseek(disco,ebr.part_start,SEEK_SET);
                                fwrite(&ebr,sizeof(EBR),1,disco);
                                printf("Particion logica creada: %s \n", name);
                            }else{
                                printf("Espacio insuficiente dentro de la particion Extendida\n");
                            }
                        }else{
                            int inicio = byte_start+ebr.part_size;
                            strcpy(ebr.part_name,name);
                            ebr.part_fit = fit;
                            ebr.part_size= size;
                            ebr.part_status= 'E';
                            fseek(disco,byte_start,SEEK_SET);
                            fwrite(&ebr, sizeof(EBR), 1, disco);
                            int tam = size-sizeof(EBR);
                            fseek(disco,inicio+sizeof(EBR),SEEK_SET);
                            while(tam>0){
                                char c = 'z';
                                fwrite(&c, 1,1,disco);
                                tam--;
                            }
                            printf("Particion logica creada: %s \n", name);
                       }
                    }else{
                        printf("Espacio insuficiente dentro de la particion Extendida\n");
                    }
                }else{
                    printf("No existe una particion extendida\n");
                }
            }
        }
        fclose(disco);
    }else{
        printf("Disco no encontrado \n");
    }
}


int partition_disponible(MBR *mbr){
    int i = 0;
    while(i<4){
        if(mbr->mbr_partition[i].part_status=='D'){//DISABLE
            return i;
        }
        i++;
    }
    return -1;
}

int extendida_creada(MBR *mbr){
    int i = 0;
    while(i<4){
        if(mbr->mbr_partition[i].part_type=='E'){//Extended
            return 1;
        }
        i++;
    }
    return 0;

}

int primer_ajuste(int required, FILE *file){//debe estar abierto el archivo
    fseek(file, sizeof(MBR), SEEK_SET);
    int current = ftell(file);
    int freespace = 0;
    while(!feof(file)&&freespace<required){
        char c = fgetc(file);
        if(c=='\0'){
            freespace++;
        }else{
            freespace = 0;
            current = ftell(file);
        }
    }
    if(freespace>=required){
        return current;
    }else{
        return -1;
    }
}

int siguiente_particion(Partition** vector, int index, int tam){
    Partition* actual = &(vector[index]);
    int final = actual->part_start+actual->part_size-1;
    int menor = tam;
    int i = 0;
    int guardarIndex=-1;
    while(i<4){
        Partition* aux = &(vector[i]);
        if(aux->part_status=='E'){
            int final2= aux->part_start+aux->part_size-1;
            int dif = final2-final;
            if(dif<menor&&dif>0){
                menor = dif;
                guardarIndex = i;
            }
        }
        i++;
    }
    return vector[guardarIndex];
 }

void Reescribir_mbr(MBR mbr, FILE *disco){
    rewind(disco);
    fwrite(&mbr, sizeof(MBR), 1, disco);
}

MBR leerMBR(FILE* disco){
    MBR mbr;
    fread(&mbr, sizeof(MBR), 1, disco);
    return mbr;
}

int indice_extendida(MBR *mbr){
    int i = 0;
    while(i<4){
        if(mbr->mbr_partition[i].part_type=='E'){//Extended
            return i;
        }
        i++;
    }
    return -1;
}


MountedDisk *b_disco_montado(char *path){
    MountedDisk *aux = montados.first;
    while(aux!=NULL){
        if(!strcmp(aux->path,path)){
            return aux;
        }
        aux = aux->siguiente;
    }
    return -1;
}

void REP(char *path, char *id, char *name){
    if(!strcasecmp(name,"mbr")){
        rep_mbr(id, path);
    }else if(!strcasecmp(name,"disk")){
        rep_disk(id,path);
    }else if(!strcasecmp(name,"bm_avd")){
        rep_avd(id,path);
    }else if(!strcasecmp(name,"bm_dd")){
        rep_dd(id,path);
    }else if(!strcasecmp(name,"bm_inode")){
        rep_inode(id,path);
    }else if(!strcasecmp(name,"bm_block")){
        rep_block(id,path);
    }else if(!strcasecmp(name,"tree")){
        rep_tree(id,path);
    }else if(!strcasecmp(name,"sb")){
        rep_sb(id,path);
    }else if(!strcasecmp(name,"avds")){
        rep_avds(id,path);
    }else if(!strcasecmp(name,"Log")){
        rep_Logs(id,path);
    }else{
        printf("Especifique un reporte");
    }
}
void rep_inode(char *id, char *path){

}
void rep_tree(char *id, char *path){}
void rep_avds(char *id, char *path){}
void rep_Logs(char *id, char *path){}
void rep_sb(char *id, char *path){
    char letra = id[2];
    char num = id[3];
    MountedDisk *disc = montados.first;
    while(disc!=NULL&&disc->letter!=letra){
        disc=disc->siguiente;
    }
    if(disc!=NULL){
        MountedPartition* part = disc->first;
        while(part!=NULL&&part->name[3]!=num){
           part = part->siguiente;
        }
        if(part!=NULL){//si se encuentra esa particion
            FILE *file = fopen(disc->path,"rb+");
            Superbloque sb = leer_SB(file, part->bit_start);
            fclose(file);
            file =fopen("/home/javierb/Documentos/REPORTE.dot","w");
            fprintf(file, "Digraph g{ \n node[shape = record];\n");
            fprintf(file, "struct1[shape = record, label = \"{");
            fprintf(file,"{  Nombre  |  Valor  }|");
            fprintf(file,"{sb_arbol_virtual_count | %d }|", sb.sb_arbol_virtual_count);
            fprintf(file,"{sb_detalle_directorio_count | %d }|", sb.sb_detalle_directorio_count);
            fprintf(file,"{sb_inodos_count | %d }|", sb.sb_inodes_count);
            fprintf(file,"{sb_bloques_count | %d }|", sb.sb_blocks_count);
            fprintf(file,"{sb_free_arbol_virtual_count | %d }|", sb.sb_free_arbol_virtual_count);
            fprintf(file,"{sb_free_detalle_dir_count | %d }|", sb.sb_free_detalle_directorio_count);
            fprintf(file,"{sb_free_bloques_count | %d }|", sb.sb_blocks_count);
            char* t = malloc(19);
            struct tm *tlocal = localtime(&sb.sb_mtime);
            strftime(t, 19, "%d/%m/%y--%H:%M:%S", tlocal);
            fprintf(file,"{SB_MTIME | %s }|", t);
            //char* t2 = malloc(19);
            //struct tm *tlocal2 = localtime(&sb.sb_umtime);
            //strftime(t2, 19, "%d/%m/%y--%H:%M:%S", tlocal2);
            //fprintf(file,"{sb_UMTIME | %s }|", sb.sb_umtime);
            fprintf(file,"{sb_MNT_COUNT | %d }|", sb.sb_mnt_count);
            fprintf(file,"{sb_magic | %d }|", sb.sb_magic);
            fprintf(file,"{sb_avd_size | %d }|", sb.sb_avd_size);
            fprintf(file,"{sb_detalle_directorio_size | %d }|", sb.sb_detalle_directorio_size);
            fprintf(file,"{sb_inode_size| %d }|", sb.sb_inode_size);
            fprintf(file,"{sb_block_size | %d }|", sb.sb_block_size);
            fprintf(file,"{sb_ap_avd | %d }|", sb.sb_ap_avd);
            fprintf(file,"{sb_ap_bitmap_avd | %d }|", sb.sb_ap_bitmap_avd);
            fprintf(file,"{sb_ap_detalle_dir | %d }|", sb.sb_ap_detalle_directorio);
            fprintf(file,"{sb_ap_bitmap_detalle_dir | %d }|", sb.sb_ap_bitmap_detalle_directorio);
            fprintf(file,"{sb_ap_tabla_inodo | %d }|", sb.sb_ap_tabla_inodo);
            fprintf(file,"{sb_ap_bitmap_tabla_inodo| %d }|", sb.sb_ap_bitmap_tabla_inodo);
            fprintf(file,"{sb_ap_bloque | %d }|", sb.sb_ap_bloque);
            fprintf(file,"{sb_ap_bitmap_tabla_inodo | %d }|", sb.sb_ap_bitmap_tabla_inodo);
            fprintf(file,"{sb_ap_log | %d }|", sb.sb_ap_log);
            fprintf(file,"{sb_ap_copia_SuperBloque | %d }|", sb.sb_ap_copia_super_bloque);
            fprintf(file,"{sb_first_free_bit_avd | %d }|", sb.sb_first_free_bit_avd);
            fprintf(file,"{sb_first_free_bit_detalle_directorio | %d }|", sb.sb_first_free_bit_detalle_directorio);
            fprintf(file,"{sb_first_free_bit_tabla_inodo | %d }|", sb.sb_first_free_bit_tabla_inodo);
            fprintf(file,"{sb_first_free_bit_bloques | %d }|", sb.sb_first_free_bit_bloques);
            fprintf(file, "}\"];\n");
            fprintf(file, "}");
            fclose(file);
            char cmd[100];
            limpiar_cadena(cmd,100);
            if(path[strlen(path)-1]=='f'){
                sprintf(cmd,"dot -Tpdf  /home/javierb/Documentos/REPORTE.dot -o %s",path);
                system(cmd);
            }else if(path[strlen(path)-1]=='g'){
                sprintf(cmd,"dot -Tpng  /home/javierb/Documentos/REPORTE.dot -o %s",path);
                system(cmd);
            }else{
                printf("formato no reconocido\n");
            }
        }else{
            printf ("Particion no encontrada\n");
        }
    }else{
        printf("Disco no encontrado \n");
    }
}

void rep_mbr(char *path, char *id){
    char letra = id[2];
    char num = id[3];
    MountedDisk *disc = montados.first;
    while(disc!=NULL&&disc->letter!=letra){
        disc=disc->siguiente;
    }
    if(disc!=NULL){
        MountedPartition* part = disc->first;
        while(part!=NULL&&part->name[3]!=num){
           part = part->siguiente;
        }
        if(part!=NULL){//si se encuentra esa particion
            FILE *file = fopen(disc->path,"rb+");
            rewind(file);
            MBR mbr;
            fread(&mbr, sizeof(MBR), 1, file);
            FILE* archivo = fopen("/home/javierb/Documentos/REPORTE.dot","w");
            fprintf(archivo,"Digraph g{ \n node[shape = record];\n");
            fprintf(archivo, "struct1[shape = record, label = \"{");
            fprintf(archivo,"{  Nombre  |  Valor  }|");
            fprintf(archivo,"{mbr_tamaño | %d }|",mbr.mbr_tam);
            fprintf(archivo,"{mbr_fecha_creacion | %s}|",mbr.mbr_fecha_creacion);
            fprintf(archivo,"{mbr_disk_signature | %d}|",mbr.mbr_disk_signature);
            int i = 0;
            for(i= 0;i<4;i++){
                if(mbr.mbr_partition[i].part_start!=0){
                    fprintf(archivo,"{part_status_%d | %c }|", i+1, mbr.mbr_partition[i].part_status);
                    fprintf(archivo,"{part_type_%d | %c }|", i+1, mbr.mbr_partition[i].part_type);
                    fprintf(archivo,"{part_fit_%d | %c }|", i+1, mbr.mbr_partition[i].part_fit);
                    fprintf(archivo,"{part_start_%d | %d}|", i+1, mbr.mbr_partition[i].part_start);
                    fprintf(archivo,"{part_size_%d | %d }|", i+1, mbr.mbr_partition[i].part_size);
                    fprintf(archivo,"{part_name_%d | %s }|", i+1, mbr.mbr_partition[i].part_name);
                }
            }
            fprintf(archivo, "}\"];\n");
            if(extendida_creada(&mbr)){
                int puntero = mbr.mbr_partition[indice_extendida(&mbr)].part_start;
                EBR ebr;
                int n_struct=2;
                fseek(file,puntero,SEEK_SET);
                fread(&ebr,sizeof(EBR),1,file);
                fprintf(archivo, "struct%d[shape = record, label = \"{",n_struct);
                fprintf(archivo,"{  Nombre  |  Valor  }|");
                fprintf(archivo,"{ebr_size_%d | %d }|",n_struct-1,ebr.part_size);
                fprintf(archivo,"{ebr_name_%d | %s }|",n_struct-1,ebr.part_name);
                fprintf(archivo,"{ebr_start_%d | %d }|",n_struct-1,ebr.part_start);
                fprintf(archivo,"{ebr_status_%d | %c }|",n_struct-1,ebr.part_status);
                fprintf(archivo,"{ebr_next_%d | %d }|",n_struct-1,ebr.part_next);
                fprintf(archivo,"{ebr_fit_%d | %c }|",n_struct-1,ebr.part_fit);
                fprintf(archivo, "}\"];\n");
                puntero = ebr.part_next;
                n_struct++;
                while(puntero>0){
                    fseek(file,puntero,SEEK_SET);
                    fread(&ebr,sizeof(EBR),1,file);
                    fprintf(archivo, "\nstruct%d[shape = record, label = \"{",n_struct);
                    fprintf(archivo,"{  Nombre  |  Valor  }|");
                    fprintf(archivo,"{ebr_size_%d | %d }|",n_struct-1,ebr.part_size);
                    fprintf(archivo,"{ebr_name_%d | %s }|",n_struct-1,ebr.part_name);
                    fprintf(archivo,"{ebr_start_%d | %d }|",n_struct-1,ebr.part_start);
                    fprintf(archivo,"{ebr_status_%d | %c }|",n_struct-1,ebr.part_status);
                    fprintf(archivo,"{ebr_next_%d | %d }|",n_struct-1,ebr.part_next);
                    fprintf(archivo,"{ebr_fit_%d | %c }|",n_struct-1,ebr.part_fit);
                    fprintf(archivo, "}\"];\n");
                    puntero = ebr.part_next;
                    n_struct++;
                }
            }
            fprintf(archivo, "}");
            fclose(archivo);
            fclose(file);
            char cmd[100];
            limpiar_cadena(cmd,100);
            if(path[strlen(path)-1]=='f'){
                sprintf(cmd,"dot -Tpdf  /home/javierb/Documentos/REPORTE.dot -o %s",path);
                system(cmd);
            }else if(path[strlen(path)-1]=='g'){
                sprintf(cmd,"dot -Tpng  /home/javierb/Documentos/REPORTE.dot -o %s",path);
                system(cmd);
            }else{
                printf("formato no reconocido\n");
            }
        }else{
            printf("Particion no encontrada\n");
        }
    }else{
        printf("Disco no encontrado \n");
    }
}


void rep_disk(char *id, char *path){
    char letra = id[2];
    char num = id[3];
    MountedDisk *disc = montados.first;
    while(disc!=NULL&&disc->letter!=letra){
        disc=disc->siguiente;
    }
    if(disc!=NULL){
        MountedPartition* part = disc->first;
        while(part!=NULL&&part->name[3]!=num){
           part = part->siguiente;
        }
        if(part!=NULL){//si se encuentra esa particion
            FILE *file = fopen(disc->path,"rb+");
            rewind(file);
            MBR mbr;
            fread(&mbr, sizeof(MBR), 1, file);
            FILE* archivo = fopen("/home/javierb/Documentos/REPORTE.dot","w");
            fprintf(archivo,"Digraph g{ \n node[shape = record];\n");
            fprintf(archivo, "struct1[shape = record, label = \"MBR|");
            int j=0;
            for(j=0;j<4;j++){
                imprimirParticion(&mbr.mbr_partition[j],archivo,file);
            }
            fprintf(archivo, "}\"];\n}");
            fclose(archivo);
            fclose(file);
            char* carp = carpeta(path);
            char cmd[100];
            limpiar_cadena(cmd,100);
            if(path[strlen(path)-1]=='f'){
                sprintf(cmd,"dot -Tpdf  /home/javierb/Documentos/REPORTE.dot -o %s",path);
                system(cmd);
            }else if(path[strlen(path)-1]=='g'){
                sprintf(cmd,"dot -Tpng  /home/javierb/Documentos/REPORTE.dot -o %s",path);
                system(cmd);
                limpiar_cadena(cmd,100);
                //(sprintf(cmd,"showtell -%s",carp);
                system(cmd);
            }else{
                printf("formato no reconocido\n");
            }
        }else{
            printf("Particion no encontrada\n");
        }
     }
    else{
     printf("Disco no encontrado \n");
    }
}
void rep_avd(char *id, char *path){
    char letra = id[2];
    char num = id[3];
    MountedDisk *disc = montados.first;
    while(disc!=NULL&&disc->letter!=letra){
        disc=disc->siguiente;
    }
    if(disc!=NULL){
        MountedPartition* part = disc->first;
        while(part!=NULL&&part->name[3]!=num){
           part = part->siguiente;
        }
        if(part!=NULL){//si se encuentra esa particion
            FILE *disco = fopen(disc->path,"rb+");
            Superbloque sb = leer_SB(disco, part->bit_start);
            char bitmap[sb.sb_arbol_virtual_count];
            fseek(disco, sb.sb_ap_bitmap_avd, SEEK_SET);
            fread(bitmap,sizeof(char), sb.sb_arbol_virtual_count, disco);
            FILE *reporte = fopen(path, "w");
            int i = 0;
            for(i=0;i<sb.sb_arbol_virtual_count; i++){
                putc(bitmap[i], reporte);
                if(i%35 == 0){
                    putc('\n', reporte);
                }
            }
        }
    }
}
void rep_block(char *id, char *path){
    char letra = id[2];
    char num = id[3];
    MountedDisk *disc = montados.first;
    while(disc!=NULL&&disc->letter!=letra){
        disc=disc->siguiente;
    }
    if(disc!=NULL){
        MountedPartition* part = disc->first;
        while(part!=NULL&&part->name[3]!=num){
           part = part->siguiente;
        }
        if(part!=NULL){//si se encuentra esa particion
            FILE *disco = fopen(disc->path,"rb+");
            Superbloque sb = leer_SB(disco, part->bit_start);
            char bitmap[sb.sb_blocks_count];
            fseek(disco, sb.sb_ap_bitmap_bloque, SEEK_SET);
            fread(bitmap,sizeof(char), sb.sb_blocks_count, disco);
            FILE *reporte = fopen(path, "w");
            int i = 0;
            for(i=0;i<sb.sb_blocks_count; i++){
                putc(bitmap[i], reporte);
                if(i%35 == 0){
                    putc('\n', reporte);
                }
            }
        }
    }
}

void rep_dd(char *id, char *path){
    char letra = id[2];
    char num = id[3];
    MountedDisk *disc = montados.first;
    while(disc!=NULL&&disc->letter!=letra){
        disc=disc->siguiente;
    }
    if(disc!=NULL){
        MountedPartition* part = disc->first;
        while(part!=NULL&&part->name[3]!=num){
           part = part->siguiente;
        }
        if(part!=NULL){//si se encuentra esa particion
            FILE *disco = fopen(disc->path,"rb+");
            Superbloque sb = leer_SB(disco, part->bit_start);
            char bitmap[sb.sb_blocks_count];
            fseek(disco, sb.sb_ap_bitmap_bloque, SEEK_SET);
            fread(bitmap,sizeof(char), sb.sb_blocks_count, disco);
            FILE *reporte = fopen(path, "w");
            int i = 0;
            for(i=0;i<sb.sb_blocks_count; i++){
                putc(bitmap[i], reporte);
                if(i%35 == 0){
                    putc('\n', reporte);
                }
            }
        }
    }
}

void imprimirParticion(Partition *part, FILE *file, FILE *disco){
    if(part->part_status=='E'){
        if(part->part_type=='P'){
            fprintf(file, "%s | ",part->part_name);
        }else{
            fprintf(file,"{%s (extendida)|{",part->part_name);
            int puntero= part->part_start;
            EBR ebr;
            fseek(disco,puntero,SEEK_SET);
            fread(&ebr,sizeof(EBR),1,disco);
            fprintf(file,"EBR| %s|",ebr.part_name);
            puntero = ebr.part_next;
            while(puntero>0){
                fseek(disco,puntero,SEEK_SET);
                fread(&ebr,sizeof(EBR),1,disco);
                fprintf(file,"EBR| %s|",ebr.part_name);
                puntero = ebr.part_next;
            }
            fprintf(file,"libre}|}|");
        }
    }else{
        fprintf(file, "LIBRE | ");
    }
}

void MKFS(char *id, char *type){
    //buscar disco
    MountedDisk *disco_montado = montados.first;
    while(disco_montado!=NULL&&!disco_montado->letter==id[2]){
        disco_montado = disco_montado->siguiente;
    }
    if(disco_montado!=NULL){//Buscando particion
        MountedPartition *particion_montada = disco_montado->first;
        while(particion_montada!=NULL&&!particion_montada->name[3]==id[3]){
            particion_montada = particion_montada->siguiente;
        }
        if(particion_montada!=NULL){
            //AQUI EMPIEZA SI ENCUENTRA LA PARTICION MONTADA
            int bit_start = particion_montada->bit_start;
            Partition particion = buscar_particion_nombre(disco_montado->path, particion_montada->nombre_part);
            int N = (particion.part_size-240)/391;
            if(N>=1){
                FILE* disco = fopen(disco_montado->path, "rb+");
                Superbloque superb = new_Superbloque(bit_start,particion.part_size);//Inizializar bloque
                escribir_superbloque(disco, &superb, bit_start);
                int i = N;
                int puntero = bit_start+sizeof(Superbloque);
                fseek(disco,superb.sb_ap_bitmap_avd, SEEK_SET);
                while(i>0){
                    fwrite('0',1,1,disco);
                    i--;
                }//bitmap de arbol
                i=N;
                avd nodo_arbol= new_avd("");
                fseek(disco,superb.sb_ap_avd, SEEK_SET);
                while(i>0){
                   fwrite(&nodo_arbol,sizeof(avd),1,disco);//arbol de directorios
                   i--;
                }
                i = N;
                fseek(disco,superb.sb_ap_bitmap_detalle_directorio, SEEK_SET);
                while(i>0){
                    fwrite('0',1,1,disco);//bitmap de detalle directorio
                    i--;
                }
                i=N;
                D_Directorio detalle;
                fseek(disco,superb.sb_ap_detalle_directorio, SEEK_SET);
                while(i>0){
                   fwrite(&detalle,sizeof(D_Directorio),1,disco);//Detalle de directorios
                   i--;
                }
                i = N;
                fseek(disco,superb.sb_ap_bitmap_tabla_inodo, SEEK_SET);
                while(i>0){
                    fwrite('0',1,1,disco);//bitmap de Inodos
                    i--;
                }
                i=N;
                Inodo inode;
                fseek(disco, superb.sb_ap_tabla_inodo, SEEK_SET);
                while(i>0){
                   fwrite(&inode,sizeof(Inodo),1,disco);//Inodos
                   i--;
                }
                i = 2*N;
                fseek(disco, superb.sb_ap_bitmap_bloque, SEEK_SET);
                while(i>0){
                    fwrite('0',1,1,disco);//bitmap de bloques
                    i--;
                }
                i=2*N;
                Data_Block bloque;
                fseek(disco, superb.sb_ap_bloque, SEEK_SET);
                while(i>0){
                   fwrite(&bloque,sizeof(Data_Block),1,disco);//Bloques
                   i--;
                }
                i=N;
                Log log;
                fseek(disco, superb.sb_ap_log, SEEK_SET);
                while(i>0){
                    fwrite(&log, sizeof(Log),1,disco);//Log de operaciones
                    i--;
                }
                fseek(disco, superb.sb_ap_copia_super_bloque, SEEK_SET);
                escribir_superbloque(disco, &superb, superb.sb_ap_copia_super_bloque);//Escribir copia de super bloque
                crear_raiz(disco, bit_start);
                fclose(disco);
                printf("Particion Formateada\n");
            }else{
              printf("Espacio insuficiente");
                //error bloques insuficientes
            }
        }else{
            printf("Particion no montada");
            //error si no esta la particion pedida
        }
    }else{
        printf("Disco no montado");
        //error si no esta el Disco
    }
}

EBR buscar_logica_nombre(char *disc, char *name){
    FILE *disco;
    EBR part;
    part.part_size =0;
    disco = fopen(disc,"rb+");
    if(disco!=NULL){
        MBR mbr;
        fread(&mbr, sizeof(MBR),1,disco);
        if(extendida_creada(&mbr)){
            int index = mbr.mbr_partition[indice_extendida(&mbr)].part_start;
            fseek(disco,index, SEEK_SET);
            fread(&part, sizeof(EBR),1,disco);
            while(part.part_next!=-1){
                if(!strcasecmp(name,part.part_name)){
                    return part;
                }
                index=part.part_next;
                fseek(disco,index, SEEK_SET);
                fread(&part, sizeof(EBR),1,disco);
            }
            return part;
        }
    }
    return part;
}

void escribir_superbloque(FILE* disco, Superbloque* superb, int bit_start){
    fseek(disco, bit_start,SEEK_SET);
    fwrite(superb, sizeof(Superbloque), 1, disco);
}

void MKFILE(char *id, char* path, int p, int size, char *cont){
    FILE *disco;
    int abrir = fopen(path, "rb+");
    if(abrir!=NULL){
        MountedDisk *disco_montado = montados.first;
        while(disco_montado!=NULL&&!disco_montado->letter==id[2]){
            disco_montado = disco_montado->siguiente;
        }
        if(disco_montado!=NULL){//Buscando particion
            MountedPartition *particion_montada = disco_montado->first;
            while(particion_montada!=NULL&&!particion_montada->name[3]==id[3]){
                particion_montada = particion_montada->siguiente;
            }
            if(particion_montada!=NULL){
                if(existe_carpeta(disco,particion_montada->bit_start, path)||p){

                }
            }
         }
    }
}

void MKDIR(char *id, char *path, int p){
    FILE *disco;
    int abrir = fopen(path, "rb+");
    if(abrir!=NULL){
        MountedDisk *disco_montado = montados.first;
        while(disco_montado!=NULL&&!disco_montado->letter==id[2]){
            disco_montado = disco_montado->siguiente;
        }
        if(disco_montado!=NULL){//Buscando particion
            MountedPartition *particion_montada = disco_montado->first;
            while(particion_montada!=NULL&&!particion_montada->name[3]==id[3]){
                particion_montada = particion_montada->siguiente;
            }
            if(particion_montada!=NULL){
                if(existe_padre(disco, particion_montada->bit_start,path)){
                    crear_carpeta(disco, particion_montada->bit_start, path);
                }else if(p){
                    crear_carpeta(disco, particion_montada->bit_start, path);
                }else{
                    printf("La carpeta padre no existe");
                }
            }
         }
    }
}
