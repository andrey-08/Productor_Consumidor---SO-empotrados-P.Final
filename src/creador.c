#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/sem.h>
#include <string.h>
#include <semaforo.h>
#include <util.h>
#include <memoria.h>

int main(int argc, char *argv[]){

	printc("--------------- CREADOR --------------- \n", 2);

	/* VALIDACION DE ARGUMENTOS */

	if(argc != 3){
        printc("Numero incorrecto de argumentos. ./creador <nombreBuffer> <tamano>\n", 1);
        exit(0);
    }

    char *nombreBuffer = argv[1];
    int sizeBuffer = atoi(argv[2]);

    if (!isNumber(argv[2])){
        printc("Tamaño invalido. Debe ser un numero entero.\n", 1);
        exit(0);
    }

    /* CREACION DE DIRECTORIOS */
    char *dir = "buffers";
    if (!checkDir(dir)){
        if (!makeDir(dir)){
            printf("Error al crear directorio: %s\n", dir);
        }else
            printf("Se ha creado el directorio: %s\n", dir);
    }

    char *nameDir = concat("buffers/", nombreBuffer);
    
    if (!checkDir(nameDir)){
        if (!makeDir(nameDir)){
            printf("Error al crear carpeta: %s\n", nameDir);
        }else
            printf("Se ha creado la carpeta: %s\n", nameDir);
    }else{
        printc("El Nombre de Buffer seleccionado ya existe\n", 1);
        exit(0);
    }

    //Creacion de buffer LS
    if (!checkDir("buffers/ls")){
        if (!makeDir("buffers/ls")){
            printf("Error al crear carpeta: %s\n", "buffers/ls");
        }else
            printf("Se ha creado la carpeta: %s\n", "buffers/ls");
    }

    /* CREACION DE KEY */
	key_t keyB = ftok(nameDir, 's');
	if(keyB == -1){
        printc("Error en la creacion de la llave \n", 1);
        exit(0);
    }

    /* CREACIONN DE BUFFER */
    int idM;
    message *memoria = NULL;
    if (!makeMem(&idM, keyB, sizeBuffer, &memoria)){
        printc("Error al crear la memoria \n", 1);
        exit(0);
    }else{
        printf("Se creo el buffer %s de tamaño %d correctamente\n\n", nombreBuffer, sizeBuffer);
    }

    /* CREACION MEMORIA DE VARIABLES GLOBALES */
    var_global *memoriaVg = NULL;
    int idV = 0;
    if(!memoryV(&idV, &memoriaVg)){
        printc("Error en variables globales.\n", 1);
        exit(0);
    }

    memoriaVg[0].productores = 0;
    memoriaVg[0].consumidores = 0;
    memoriaVg[0].tam = 0;  
    memoriaVg[0].tam2=0;
    memoriaVg[0].sizeMem=sizeBuffer;
    memoriaVg[0].fin = 0;
    memoriaVg[0].consumidos = 0;
    memoriaVg[0].producidos = 0;
    memoriaVg[0].key_deleted = 0;
    memoriaVg[0].esperaTotal = 0;
    memoriaVg[0].bloqTotal = 0;
    memoriaVg[0].usoTotal = 0;
    memoriaVg[0].totalKernel = 0;
    memoriaVg[0].totalConsumidores = 0;
    memoriaVg[0].endFinalizer = 0;

    /* INICIALIZACION DE SEMAFOROS */
    int mem, vacio, lleno;

    borrarSemIni(mem);
    borrarSemIni(vacio);
    borrarSemIni(lleno);

    // generan el id del semaforo
    mem = makeSem(nameDir, 1, 1, 1);             // semaforo para zoa critica, iniciallizado en 1
    vacio = makeSem(nameDir, 2, 1, sizeBuffer);  // semaforo para indicar cuando no haya espacio para agregar, inic. en tamano buffer
    lleno = makeSem(nameDir, 3, 1, 0);			  // Semaaforo que indica que hay algo para consumir, iniciallizado en 0


    /* INICIALIZAR MEMORIA */
    for (int i = 0; i < sizeBuffer; i++){
        memoria[i].pid = 0;
        memoria[i].magic_number = 0;
        strcpy(memoria[i].date, "");
        strcpy (memoria[i].hour, "");
        strcpy (memoria[i].text, "");
    }


}