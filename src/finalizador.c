#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/sem.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <semaforo.h>
#include <util.h>
#include <memoria.h>

message *buffer = NULL;
var_global *variables = NULL;

int main(int argc, char *argv[]){



	if(argc != 2){
        printc("Argumentos incorrects. Introducir: ./finalizador <nombreBuffer>\n", 1);
        exit(0);               
    }

    printc("\n----------------- Finalizador -----------------\n", 2);

    char *nombreBuffer = argv[1];

    char *dirB = concat("buffers/",argv[1]);
    key_t keyB;

    if (!checkDir(dirB)) {
        printc("Buffer no existe\n", 1);
        exit(0);
    }
    else{
        keyB =  ftok (dirB, 's');
        if (keyB == -1){
            printc("No se pudo generar clave del buffer \n", 1);
            exit(0);
        }
    }

    int idV = 0;
    if (!memoryV(&idV, &variables)){
        printc("No se pudo obtener variables globales\n", 1);
        exit(0);
    }

    int idM;
    if (!makeMem(&idM, keyB, variables[0].tam, &buffer)){
        printc("No se pudo obtener el buffer.\n", 1);
        exit(0);
    }


    /* APERTURA DE SEMAFOROS */
    char *nameDir = concat("buffers/", nombreBuffer);
    int mem, vacio, lleno;
    mem = openSem(nameDir, 1, 1);
    vacio = openSem(nameDir, 2, 1);
    lleno = openSem(nameDir, 3, 1);

    variables[0].fin = -1; // Le indica a consumidor y productor que deben finalizar


    /* BORRAR MEMORIA VARIABLES GLOBALES */
    if(!borrarMemoria(idV)){
        printc("No se pudo borrar memoria de vairables globales.\n", 1);
    }

    /* BORRAR MEMORIA DE BUFFFERS */
    if(!borrarMemoria(idM)){
        printc("No se pudo borrar la memoria de los buffers.\n", 1);
    }

    /* BORRA LOS SEMAFOROS */
    if(!borrarSem(mem)){
        char close[50];
        sprintf(close,"-> ID del semaforo eliminado: %i \n", mem);
        printc(close, 5);
    }
    if(!borrarSem(vacio)){
        char close1[50];
        sprintf(close1,"-> ID del semaforo eliminado: %i \n", vacio);
        printc(close1, 5);
    }
    if(!borrarSem(lleno)){
        char close2[50];
        sprintf(close2,"-> ID del semaforo eliminado: %i \n", lleno);
        printc(close2, 5);
    }

   	/* ELIMINA EL DIRECTORIO CREADO */
    if (checkDir(nameDir)){
        system(concat("rm -rf ", nameDir));
        char out[70];
        sprintf(out, "-> Directorio %s ha sido eliminado.\n", nombreBuffer);
        printc(out, 5);
    }

    while(1){
        if (variables[0].endFinalizer == 0){
            printf("\n");
		    printc("****************** Estadisticas ******************* \n", 2);

            char out2[150];
            sprintf(out2,"- Productores totales: %i\n- Consumidores totales: %i\n", variables[0].productores, variables[0].totalConsumidores);
            printc(out2,5);
            char out3[100];
            sprintf(out3, "- Total de mensajes producidos: %i\n- Total de mensajes consumidos: %i\n", variables[0].producidos, variables[0].consumidos);
		    printc(out3,5);

            char out4[100];
		    sprintf(out4, "- Consumidores eliminados por magic number:  %i \n\n", variables[0].key_deleted);
		    printc(out4,5);
            exit(0);
        }
    }
    
    return 0;

}