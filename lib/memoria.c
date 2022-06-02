#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <util.h>

#include "../include/memoria.h"

// http://www.chuidiang.org/clinux/ipcs/mem_comp.php


/* CREACION DE MEMORIA COMPARTIDA */
int makeMem(int *shmid, key_t key, int size, message **Memoria){
  if ((*shmid = shmget(key, sizeof(message)*size, 0777 | IPC_CREAT)) < 0) {  //Para crear zona de memoria y devolver un identificador de esta
    printc("Error de shmget() en el buffer\n", 1);
    return 0;
   } 
   if ((*Memoria=( message *)shmat(*shmid, (char *)0, 0)) == (void *) -1) {  // Devuelve un puntero que apunte a zona de memoria
    printc("Error de shmat() en el buffer\n", 1);
    return 0;
   }
   return 1;
}

/* MEMORIA COMPARTIDA DE VARIABLES GLOBALES */

int memoryV(int *shmid, var_global **memoria){
  key_t key = ftok ("/bin", 33);
	if (key == -1){
		printc("Error en la clave de las variables globales\n", 1);
		return 0;
	}
  if ((*shmid=shmget(key, sizeof(var_global), 0777 | IPC_CREAT))<0) {
    printc("Error de shmget() en el buffer de las variables globales\n", 1);
    return 0;
   } 
   if ((*memoria=( var_global *)shmat(*shmid, (char *)0, 0)) == (void *) -1) {
    printc("Error de shmat() en el buffer de las variables globales\n", 1);
    return 0;
   }
   return 1;
}

/* BORRAR MEMORIA */
int borrarMemoria(int shmid){
  if (shmctl(shmid, IPC_RMID, 0) < 0) {
    return 0;
  } else
    return 1;
}