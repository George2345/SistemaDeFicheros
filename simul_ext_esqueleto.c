#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include "cabeceras.h"

#define LONGITUD_COMANDO 100

void Printbytemaps(EXT_BYTE_MAPS *ext_bytemaps);
int ComprobarComando(char *strcomando, char *orden, char *argumento1, char *argumento2);
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
   int aux = 0, aux2 = 0, aux3 = 0;
   for (int i = 0; strcomando[i] != '\0'; i++){
      if (strcomando[i] != ' ' && aux == 0){
         orden[i] = strcomando[i];
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
      }
   }
   for (int i = 0; orden[i] != '\0'; i++){
      printf("%c", orden[i]);
   }
   printf("\n");
   for (int i = 0; argumento1[i] != '\0'; i++){
      printf("%c", argumento1[i]);
   }
   printf("\n");
   for (int i = 0; argumento2[i] != '\0'; i++){
      printf("%c", argumento2[i]);
   }
   if ((strcmp(orden, "info")==10 || strcmp(orden, "bytemaps")==10 || strcmp(orden, "dir")==10) && argumento1[0] == '\0' && argumento2[0] == '\0'){
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
     fread(&datosfich, SIZE_BLOQUE, MAX_BLOQUES_PARTICION, fent);    
     
     
     memcpy(&ext_superblock,(EXT_SIMPLE_SUPERBLOCK *)&datosfich[0], SIZE_BLOQUE);
     memcpy(&directorio,(EXT_ENTRADA_DIR *)&datosfich[3], SIZE_BLOQUE);
     memcpy(&ext_bytemaps,(EXT_BLQ_INODOS *)&datosfich[1], SIZE_BLOQUE);
     memcpy(&ext_blq_inodos,(EXT_BLQ_INODOS *)&datosfich[2], SIZE_BLOQUE);
     memcpy(&memdatos,(EXT_DATOS *)&datosfich[4],MAX_BLOQUES_DATOS*SIZE_BLOQUE);
     
     // Buce de tratamiento de comandos
     for (;;){
		 do {
		 printf (">> ");
		 fflush(stdin);
		 fgets(comando, LONGITUD_COMANDO, stdin);
       memset(orden, 0, LONGITUD_COMANDO);
       memset(argumento1, 0, LONGITUD_COMANDO);
       memset(argumento2, 0, LONGITUD_COMANDO);
		 } while (ComprobarComando(comando,orden,argumento1,argumento2) !=0);
	      if (strcmp(orden,"dir")==10) {
            //Directorio(&directorio,&ext_blq_inodos);
            printf("entrod\n");
            continue;
         }
         //...
         else if (strcmp(orden, "intro")==10){
            printf("entroi\n");
            //Imprimir(&directorio, &ext_blq_inodos, &memdatos, &)
         }
         else if (strcmp(orden, "bytemaps")==10){
            printf("entrob\n");
            //Renombrar(&directorio, &ext_blq_inodos, argumento1, argumento2);
         }
         else if (strcmp(orden, "rename")==0){
            printf("entror\n");
            //Renombrar(&directorio, &ext_blq_inodos, argumento1, argumento2);
         }
         else if (strcmp(orden, "copy")==0){
            printf("entroc\n");
            //Renombrar(&directorio, &ext_blq_inodos, argumento1, argumento2);
         }
         else if (strcmp(orden, "imprimir")==0){
            printf("entro\n");
            //Renombrar(&directorio, &ext_blq_inodos, argumento1, argumento2);
         }
         else if (strcmp(orden, "remove")==0){
            printf("entroim\n");
            //Renombrar(&directorio, &ext_blq_inodos, argumento1, argumento2);
         }
         /* Escritura de metadatos en comandos rename, remove, copy     
         Grabarinodosydirectorio(&directorio,&ext_blq_inodos,fent);
         GrabarByteMaps(&ext_bytemaps,fent);
         GrabarSuperBloque(&ext_superblock,fent);
         if (grabardatos)
           GrabarDatos(&memdatos,fent);
         grabardatos = 0;*/
         //Si el comando es salir se habrán escrito todos los metadatos
         //faltan los datos y cerrar
         if (strcmp(orden,"salir")==10){
            //GrabarDatos(&memdatos,fent);
            fclose(fent);
            return 0;
         }
     }
}
