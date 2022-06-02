#ifndef _MEMORIA_H
#define _MEMORIA_H

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

typedef struct{
  int productores;   // productores activos
  int consumidores;  // Consumidores activos
  int tam;			     // indice de buffer para productor
  int tam2;          // indice del buffer para el consumidor
  int sizeMem;       // Tamano del buffer
  int fin;		       // indica a proceo que se solicito finalizar  opr el finalizador
  float esperaTotal; // tiempo total que se estuvo en espera (suma de cosumidor y productor)
  float bloqTotal;   // tiempo total en bloqueo
  float usoTotal;	   // tiempo total en uso 
  float totalKernel;
  int totalConsumidores;
  int consumidos;
  int producidos;
  int key_deleted;   // total consum. cancelados por numero magico
  int endFinalizer;  // indicar cuando se han cancelado por mensaje del finalizador
} var_global;

typedef struct{
  int pid;
  int magic_number;
  char date[50];
  char hour[50];
  char text[50];
} message;

int makeMem(int *shmid, key_t key, int size, message **Memoria);
int memoryV(int *shmid, var_global **memoria);
int borrarMemoria(int shmid);

#endif