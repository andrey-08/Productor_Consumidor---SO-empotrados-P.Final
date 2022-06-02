#ifndef _SEMAFORO_H
#define _SEMAFORO_H


int makeSem(char *buffer, int identifier, int sem_num, int init_val);
int borrarSem(int id);
void borrarSemIni(int id);
int openSem(char *buffer, int identifier, int sem_num);
void upSem(int id, int num);
void downSem(int id, int num);
int sem_get_value(int id, int num);

 
#endif