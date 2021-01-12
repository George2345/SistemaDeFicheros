#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "cabeceras.h"

#define LONGITUD_COMANDO 100

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps);                                                                        //Bytemaps
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2);                                //Comprueba el comando
void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup);                                                                       
int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, 
              char *nombre);
void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos);
int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, 
              char *nombreantiguo, char *nombrenuevo);
int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, 
             EXT_DATOS *memdatos, char *nombre);
int Borrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,
           EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,
           char *nombre,  FILE *fich);
int Copiar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos,
           EXT_BYTE_MAPS *ext_bytemaps, EXT_SIMPLE_SUPERBLOCK *ext_superblock,
           EXT_DATOS *memdatos, char *nombreorigen, char *nombredestino,  FILE *fich);
void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich);
void GrabarByteMaps(EXT_BYTE_MAPS *ext_bytemaps, FILE *fich);
void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich);
void GrabarDatos(EXT_DATOS *memdatos, FILE *fich);

int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2){
   int flag = 1;
   int aux = 0, aux1 = 0, aux2 = 0, aux3 = 0;
   for (int i = 0; strcomando[i] != '\0'; i++){
      if (strcomando[i] != ' ' && aux == 0 && (strcomando[i] >= 'a' && strcomando[i] <= 'z')){
         orden[aux1] = strcomando[i];
         aux1++;
      }
      else if (strcomando[i] != ' ' && aux == 1){
         argumento1[aux2] = strcomando[i];
         aux2++;
      }
      else if (strcomando[i] != ' ' && aux == 2){
         argumento2[aux3] = strcomando[i];
         aux3++;
      }
      if (strcomando[i] == ' '){
         aux++;
         if (aux == 1 || aux == 2){
            strcomando[strlen(strcomando)-1] = ' '; 
         }
      }
   }
   /*printf("%d\n", strlen(strcomando));
   printf("%d\n", strlen(orden));
   printf("%d\n", strlen(argumento1));
   printf("%d\n", strlen(argumento2));*/
   if ((strcmp(orden, "info")==0 || strcmp(orden, "bytemaps")==0 || strcmp(orden, "dir")==0 || strcmp(orden, "salir")==0) && argumento1[0] == '\0' && argumento2[0] == '\0'){
      flag = 0;
   }
   else if ((strcmp(orden, "rename")==0 || strcmp(orden, "copy")==0) && argumento1[0] != '\0' && argumento2[0] != '\0'){
      flag = 0;
   }
   else if ((strcmp(orden, "imprimir")==0 || strcmp(orden, "remove")==0) && argumento1[0] != '\0' && argumento2[0] == '\0'){
      flag = 0;
   }

   if (flag == 1) {
      printf("ERROR: Comando ilegal [bytemaps, copy, dir, info, imprimir, rename, remove, salir]\n");
   }
   return flag;
}

void LeeSuperBloque(EXT_SIMPLE_SUPERBLOCK *psup){
   printf("Bloque %d Bytes\n", psup->s_block_size);
   printf("inodos particion = %d\n", psup->s_inodes_count);
   printf("inodos libres = %d\n", psup->s_free_inodes_count);
   printf("Bloques particion = %d\n", psup->s_blocks_count);
   printf("Bloques libres = %d\n", psup->s_free_blocks_count);
   printf("Primer bloque de datos = %d\n", psup->s_first_data_block);
}  

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps) {
   printf("Inodos :");
   for (int i = 0; i < MAX_INODOS; i++){
      printf("%d ", ext_bytemaps->bmap_inodos[i]);
   }
   printf("\nBloques [0-25] :");
   for (int i = 0; i < 25; i++){
      printf("%d ", ext_bytemaps->bmap_bloques[i]);
   }
   printf("\n");
} 

void Directorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos){
   for (int i = 1; i < sizeof(directorio); i++){
      printf("%s", directorio[i].dir_nfich);
      printf("\ttamanio:%d", inodos->blq_inodos[directorio[i].dir_inodo].size_fichero);
      printf("\tinodo:%d\t bloques: ", directorio[i].dir_inodo);
      for (int j = 0; j < MAX_NUMS_BLOQUE_INODO; j++){
         if (inodos->blq_inodos[directorio[i].dir_inodo].i_nbloque[j] != 65535){
            printf("%d ", inodos->blq_inodos[directorio[i].dir_inodo].i_nbloque[j]);
         }
      }
      printf("\n");
   }  
}

int Renombrar(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombreantiguo, char *nombrenuevo){
   int flag = 0, aux = 1;

   //Comprobacion de la existencia de los ficheros
   for (int i = 1; i < sizeof(directorio); i++){
      if (strcmp(nombreantiguo, directorio[i].dir_nfich)!=0){
         aux++;
      }
      if (strcmp(nombrenuevo, directorio[i].dir_nfich)==0){
         printf("ERROR: El fichero %s ya existe\n", nombrenuevo);
         flag = 1;
         break;
      }
      if (aux == sizeof(directorio)){
         printf("ERROR: Fichero %s no encontrado\n", nombreantiguo);
         flag = 1;
      }
   }
   if (flag == 0){
      for (int i = 1; i < sizeof(directorio); i++){
         if (strcmp(nombreantiguo, directorio[i].dir_nfich)==0){
            strcpy(directorio[i].dir_nfich, nombrenuevo);
         }
      }
   }

   return flag;
}

int BuscaFich(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, char *nombre){
   int flag = 0, aux = 1;
   
   for (int i = 1; i < sizeof(directorio); i++){
      if (strcmp(nombre, directorio[i].dir_nfich)!=0){
         aux++;
      }
      if (aux == sizeof(directorio)){
         printf("ERROR: Fichero %s no encontrado\n", nombre);
         flag = 1;
      }
   }
   return flag;
}

int Imprimir(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, EXT_DATOS *memdatos, char *nombre) {
   int flag = 0;
   int buscaFich = BuscaFich(directorio, inodos, nombre);
   char a[50] = "hola que ase";
   char b[50] = "adios muy buenas";
   puts(b);
   //memset(b, 0, strlen(b));
   memcpy(b, a, strlen(a));
   puts(b);
   /*if (buscaFich == 0){
      memset(memdatos->dato, 0, SIZE_BLOQUE);
      for (int i = 1; i < sizeof(directorio); i++){
         if (strcmp(nombre, directorio[i].dir_nfich)==0){
            memcpy(&memdatos->dato, &inodos->blq_inodos[directorio[i].dir_inodo].i_nbloque, 1);
         }
      }
   }
   puts(memdatos->dato);*/
   
   return flag;
}


void Grabarinodosydirectorio(EXT_ENTRADA_DIR *directorio, EXT_BLQ_INODOS *inodos, FILE *fich){
   printf("Graba directorio e inodos\n");
   FILE *f;
   int a = 14;
   f = fopen("b.bin", "r+b");
   fwrite(&a, sizeof(a), 1, f);
   /*for (int i = 1; i < sizeof(directorio); i++){
      fwrite(&directorio[i].dir_nfich, sizeof(directorio[i].dir_nfich), strlen(directorio[i].dir_nfich), f);
   }*/
}

void GrabarSuperBloque(EXT_SIMPLE_SUPERBLOCK *ext_superblock, FILE *fich){
   //ext_superblock->s_free_blocks_count--;
   fwrite(&ext_superblock->s_free_blocks_count, sizeof(ext_superblock->s_free_blocks_count), 1, fich);
}


int main()
{
	char comando[LONGITUD_COMANDO];
	char orden[LONGITUD_COMANDO];
	char argumento1[LONGITUD_COMANDO];
	char argumento2[LONGITUD_COMANDO];
	 
	int i,j;
	unsigned long int m;
   EXT_SIMPLE_SUPERBLOCK ext_superblock;
   EXT_BYTE_MAPS ext_bytemaps;
   EXT_BLQ_INODOS ext_blq_inodos;
   EXT_ENTRADA_DIR directorio[MAX_FICHEROS];
   EXT_DATOS memdatos[MAX_BLOQUES_DATOS];
   EXT_DATOS datosfich[MAX_BLOQUES_PARTICION];
   int entradadir;
   int grabardatos;
   FILE *fent;
   
   // Lectura del fichero completo de una sola vez
   //...
   
   fent = fopen("particion.bin","r+b");

   //Comprobacion de la existencia del fichero
   if (fent == NULL){
        printf("El fichero no existe\n");
        return 0;
    }
   fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fent);    
   
   
   memcpy(&ext_superblock,(EXT_SIMPLE_SUPERBLOCK *)&datosfich[0], SIZE_BLOQUE);
   memcpy(&directorio,(EXT_ENTRADA_DIR *)&datosfich[3], SIZE_BLOQUE);
   memcpy(&ext_bytemaps,(EXT_BLQ_INODOS *)&datosfich[1], SIZE_BLOQUE);
   memcpy(&ext_blq_inodos,(EXT_BLQ_INODOS *)&datosfich[2], SIZE_BLOQUE);
   memcpy(&memdatos,(EXT_DATOS *)&datosfich[4],MAX_BLOQUES_DATOS*SIZE_BLOQUE);
   
   // Bucle de tratamiento de comandos
   for (;;){
      do {
      printf (">> ");
      fflush(stdin);
      fgets(comando, LONGITUD_COMANDO, stdin);

      //Limpieza de la cadena para que no lo una con basura
      memset(orden, 0, LONGITUD_COMANDO);
      memset(argumento1, 0, LONGITUD_COMANDO);
      memset(argumento2, 0, LONGITUD_COMANDO);
      } while (ComprobarComando(comando,orden,argumento1,argumento2) !=0);
      if (strcmp(orden,"dir")==0) {
         Directorio(directorio,&ext_blq_inodos);
         continue;
      }
      else if (strcmp(orden, "info")==0){
         LeeSuperBloque(&ext_superblock); 
         continue; 
      }
      else if (strcmp(orden, "bytemaps")==0){
         Printbytemaps(&ext_bytemaps);  
         continue;
      }
      else if (strcmp(orden, "rename")==0){
         Renombrar(directorio, &ext_blq_inodos, argumento1, argumento2);
      }
      else if (strcmp(orden, "copy")==0){
         //Renombrar(&directorio, &ext_blq_inodos, argumento1, argumento2);
      }
      else if (strcmp(orden, "imprimir")==0){
         Imprimir(directorio, &ext_blq_inodos, memdatos, argumento1);
         continue;
      }
      else if (strcmp(orden, "remove")==0){
         //Renombrar(&directorio, &ext_blq_inodos, argumento1, argumento2);
      }
      //Escritura de metadatos en comandos rename, remove, copy     
      Grabarinodosydirectorio(directorio,&ext_blq_inodos,fent);
      //GrabarByteMaps(&ext_bytemaps,fent);
      //GrabarSuperBloque(&ext_superblock,fent);
      /*if (grabardatos)
         GrabarDatos(&memdatos,fent);
      grabardatos = 0;*/
      //Si el comando es salir se habrán escrito todos los metadatos
      //faltan los datos y cerrar
      if (strcmp(orden,"salir")==0){
         //GrabarDatos(&memdatos,fent);
         fclose(fent);
         return 0;
      }
   }
}
