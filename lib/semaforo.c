 #include <sys/shm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>


/**
 * link de explicacion de uso de semaforos:
 * http://www.chuidiang.org/clinux/ipcs/semaforo.php
 * */


struct sembuf operation;

/**
 * Creacion del semaphore 
 * char* buffer --> key 
 * int identifier --> identificador
 * int sem_num --> semaforos por crear
 * int init_val --> Valor inicial, 0 is rojo y 1 verde
 * Example: sem_create("../bin/ls",33, 10,1);
*/
int makeSem(char *buffer, int identifier, int sem_num, int init_val) {
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
// The union is already defined in sys/sem.h
#else
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short int *array;
        struct seminfo *__buf;
    };
#endif

    
    union semun arg;
    int id;
    key_t key;

    key = ftok(buffer, identifier);

    if (key == -1){
        printf("No se pudo obtener la llave del semaforo\n");
        exit(0);
    }

    id = semget(key, sem_num, 0600 | IPC_CREAT);

    if (id == -1){
        printf("No se pudo crear el semaforo\n");
        exit(0);
    }

    for (int i = 0; i <= sem_num; i++){
      arg.val = init_val;
      semctl(id,i, SETVAL, arg);
    } 

    operation.sem_flg = 0;

    return id;
}

/**
 * Funcion para borrar un semaforo
 * int id --> id del set de semaforos
*/
void borrarSemIni(int id) {
    semctl(id, 2, IPC_RMID, NULL);
}

/**
 * Funcion para borrar un semaforo
 * int id --> id del set de semaforos
*/
int borrarSem(int id) {
    semctl(id, 2, IPC_RMID, NULL);
    if (errno == EINVAL){
        return 1;
    }
    return 0;
}

/**
 * Funcion para abrir un semaforo existente
 * char* buffer --> key 
 * int identifier --> identificador
 * int sem_num --> numero de semaforos
 * Example: sem_open("../bin/ls",33, 10);
*/

int openSem(char *buffer, int identifier, int sem_num) {
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
// The union is already defined in sys/sem.h
#else
    union semun {
        int val;
        struct semid_ds *buf;
        unsigned short int *array;
        struct seminfo *__buf;
    };
#endif

    //union semun arg;
    int id;
    key_t key;

    key = ftok(buffer, identifier);

    if (key == -1){
        printf("No se pudo obtener la llave en abrirSem\n");
        exit(0);
    }

    id = semget(key, sem_num, 0600);

    if (id == -1){
        printf("No se pudo abrir el semaforo en abrirSem\n");
        exit(0);
    }

    operation.sem_flg = 0;

    return id;
}

/**
 * incrementa un semaforo en 1
 * int id --> id del set se semaforos
 * int num --> numero especifico del semaforo
 * Example: sem_up(sem1,0);
*/
void upSem(int id, int num) {
    struct sembuf operation;
    operation.sem_num = num;
    operation.sem_op = 1;
    semop(id, &operation, 1);
}


/**
 * Decrementa un semaforo en en -1
 * int id --> id del set se semaforos
 * int num --> numero especifico del semaforo
*/
void downSem(int id, int num) {
    struct sembuf operation;
    operation.sem_num = num;
    operation.sem_op = -1;
    semop(id, &operation, 1);
}

/**
 * Obtener el valor de un semaforo en especifico
 * int id --> id of the set of semaphores
 * int num --> number of the especific semaphore
 * Example: sem_get_value(sem1,0);
*/
int sem_get_value(int id, int num) {
    int value;
    value = semctl(id, num, GETVAL, 0);
    return value;
}
