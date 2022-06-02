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

double expo(double lambda);
void lecturaMem(int i, int n);
void endConsumer(float tiempoUso, int consum);

message *buffer = NULL;    
var_global *variables = NULL;

int main(int argc, char *argv[]){

	if(argc != 3){
        printc("Argumentos incorrectos, forma correcta: ./consumidor <nombreBuffer> <mediaEspera>\n", 1);
        exit(0);               
    }

    char *nombreBuffer = argv[1];
    float tiempoEspera = atof(argv[2]);

    if (!isNumber(argv[2])){
        printc("Media de tiempo incorrecta. Debe ser un numero entero.\n", 1);
        exit(0);
    }

    /* CREACION DE CLAVE PARA EL BUFFER */
    char * dirB = concat("buffers/",argv[1]);
    key_t keyB;

    
    
    if (!checkDir(dirB)) {
        printc("El buffer no existe\n", 1);
        exit(0);
    }else{
        keyB =  ftok (dirB, 's');
        if (keyB == -1){
            printc("No se pudo generar clave\n", 1);
            exit(0);
            }
    }

    int idV = 0;
    if (!memoryV(&idV, &variables)){
        printc("No se pudo obtener memoria de variables globales\n", 1);
        exit(0);
    }

    int idM;
    if (!makeMem(&idM, keyB, variables[0].tam, &buffer)){
        printc("No se pudo obtener memoria compartida.\n", 1);
        exit(0);
    }

    variables[0].consumidores ++;
    variables[0].totalConsumidores ++;

    printc("\n-------------- Consumidor -------------\n", 2);

    /* APERTURA DE SEMAFOROS */
    char *nameDir = concat("buffers/", nombreBuffer);
    int mem, vacio, lleno;
    mem = openSem(nameDir, 1, 1);
    vacio = openSem(nameDir, 2, 1);
    lleno = openSem(nameDir, 3, 1);

    int consum = 0;
    int n = 1;
    float timeEsp, timeBloq;
    struct timeval t1, t2, t3, t4, start, end;

     gettimeofday(&start, NULL);

     while(1){
     	printc("------------------------------------------ \n", 3);


     	if(variables[0].fin == -1){
            gettimeofday(&end, NULL);
            float tiempoUso = (end.tv_sec - start.tv_sec);
            variables[0].usoTotal += (end.tv_sec - start.tv_sec);
            variables[0].endFinalizer=0;;
            endConsumer(tiempoUso, consum);
        }

        gettimeofday(&t3, NULL);

        float timeAleat = expo(tiempoEspera/10);
        sleep(timeAleat);

        gettimeofday(&t4, NULL);
        //printf("timepo t4 %ld\n", t4.tv_sec);

        timeEsp += timeAleat;

        gettimeofday(&t1, NULL);
        downSem(lleno,0);        //Bandera para saber si hay mensajes que leer, disminuye en 1 el sem.
        

        downSem(mem, 0);   		/** Semaforo para indicar que zona critica esta ocupada,
        						    pasa si esta en 1, si esta en 0 no*/
        gettimeofday(&t2, NULL);

        printc("Inicio de lectura\n", 3);
        
        int id = (getpid() % 5);

        
        variables[0].esperaTotal += (t4.tv_sec - t3.tv_sec);
        variables[0].bloqTotal += (t2.tv_sec - t1.tv_sec);

        timeBloq += (t2.tv_sec - t1.tv_sec);

        if(id == buffer[variables[0].tam2].magic_number){
			n=3;
			lecturaMem(variables[0].tam2, n);
			gettimeofday(&end, NULL);
			float tiempoExe2 = (end.tv_sec - start.tv_sec);
			variables[0].usoTotal += (end.tv_sec - start.tv_sec);
			printc("\n-> El Magic Number del mensaje es igual que PID % 5 <-\n", 1);
			printc("-> Se cancela este consumidor <-\n\n", 1);

            char out[150];
			sprintf(out, "- ID del consumidor: %i\n- Tiempo de ejecucion del proceso: %f\n", getpid(), tiempoExe2);
			printc(out, 5);

            consum ++;
			variables[0].consumidos ++;
            variables[0].consumidores --;
            variables[0].key_deleted ++;

            char out2[150];
            sprintf(out2,"- Mensajes consumidos: %i\n", consum);
            printc(out2,5);

            variables[0].tam2 ++;
            if(variables[0].tam2 == variables[0].sizeMem){
                variables[0].tam2 = 0; 
            }

            printc("\nProceso finalizado!!\n", 3);
        
      		upSem(mem, 0); 	//libera la zona critica
          	upSem(vacio,0);	//Semaforo que indica al roductor que hay espacio dispo, se le suma 1

          	exit(0);
        }

        lecturaMem(variables[0].tam2, n);

        char time[50];
        sprintf(time, "- Tiempo de espera para consumo: %f\n", timeAleat);
        printc(time, 5);
        consum ++;
        variables[0].consumidos ++;

        variables[0].tam2 ++;
        if(variables[0].tam2 == variables[0].sizeMem){
            variables[0].tam2 = 0; 
        }

        printc("\nLectura finalizada\n", 3);

        upSem(mem, 0); 	//libera la zona critica
        upSem(vacio,0);	//Semaforo que indica al roductor que hay espacio dispo, se le suma 1

    }

    return 0;
}

double expo(double lambda){  //Numeros aleatorios a traves de una distribucion exponencial
    double u;
    srand((unsigned)time(NULL));
    u = rand() / (RAND_MAX + 1.0);
    return -log(1- u) / (lambda);
}

void lecturaMem(int i, int n){
    printf("\n");

    char out[150];
    printc("->",3);
    sprintf(out,"Leido un mensaje del indice # %i: \n- Productores activos: %i\n- Consumidores activos: %i\n", i, variables[0].productores, variables[0].consumidores);
    printc(out,5);

    char text2[200];
    sprintf(text2, "- Mensaje: %s", buffer[i].text);
    printc(text2, 6);

    char text1[200];
    sprintf(text1, " - %s | %s \n", buffer[i].date, buffer[i].hour);
    printc(text1, 6);
} 

void endConsumer(float tiempoUso, int consum){
    printf("\n");
    printc("*********** Se ha detectado mensaje de finalizacion ************\n",1);
    char text[150];
    sprintf(text, "- Mensajes consumidos por este proceso: %i\n- Tiempo de ejecucion: %f\n\n", consum, tiempoUso);
    printc(text, 5);

    exit(0);
}