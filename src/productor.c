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
void endProducer(float tiempo, int msg, float bloq);

message *buffer = NULL;
var_global *variables = NULL;
struct timeval t1, t2, t3, t4, start, end;

int main(int argc, char *argv[]){

    if(argc != 3){
        printc("Argumentos incorrectos, forma correcta: ./productor <nombreBuffer> <mediaEspera>\n", 1);
        exit(0);               
    }

    char *nameBuffer = argv[1];
    float tiempoEspera = atof(argv[2]);

    //Verificar la media de tiempo en segundos
    if (!isNumber(argv[2])){
        printc("Media de tiempo incorrecta. Debe ser un numero entero.\n", 1);
        exit(0);
    }

    /* CLAVE DEL BUFFER */
    char * dirB = concat("buffers/",argv[1]);
    key_t keyB;
    
    if (!checkDir(dirB)) {
        printc("El buffer no existe\n", 1);
        exit(0);
    }else{
        keyB =  ftok (dirB, 's');
        if (keyB == -1){
            printc("Error al obtener la clave para la memoria compartida\n", 1);
            exit(0);
            }
    }

    /* MEMORIA DE VARIABLES GLOBALES */
    int idV = 0;
    if (!memoryV(&idV, &variables)){
        printc("Error al leer las variables globales\n", 1);
        exit(0);
    }

    /* MEMORIA BUFFER COMPARTIDO */
    int idM;
    if (!makeMem(&idM, keyB, variables[0].tam, &buffer)){
        printc("Error al obtener el buffer de memoria compartida.\n", 1);
        exit(0);
    }

    printc("\n------------- Productor ----------------- \n", 2);

    float bloqTime, waitTime;

    variables[0].productores ++;
    variables[0].endFinalizer = 1;

    /* APERTURA DE SEMAFOROS */
    char *nameDir = concat("buffers/", nameBuffer);
    int mem, vacio, lleno;
    mem = openSem(nameDir, 1, 1);
    vacio = openSem(nameDir, 2, 1);
    lleno = openSem(nameDir, 3, 1);

    int numMsg = 0;  // Para msj producidos por este proceso
    gettimeofday(&start, NULL); //inicia el contador de tiempo 

    while(1){

    	printc("------------------------------------------ \n", 4);

        gettimeofday(&t3, NULL);
        float timeAleat = expo(tiempoEspera/10);
        sleep(timeAleat);
        gettimeofday(&t4, NULL);
        waitTime += (t4.tv_sec - t3.tv_sec);


        gettimeofday(&t1, NULL);
        downSem(vacio,0);	 //Se disminuye semaforo de espacio de memoria disponible

        
        /* FINALIZACION POR MENSAJE DEL FINALIZADOR */
        if(variables[0].fin == -1){
            gettimeofday(&end, NULL);
            double tiempoT = (end.tv_sec - start.tv_sec);
            variables[0].totalKernel += (end.tv_sec - start.tv_sec);
            variables[0].endFinalizer = 0;
            endProducer(tiempoT, numMsg, bloqTime);
        }

        downSem(mem, 0);  // Se entra a memoria compartida, se disminuye semaforo de memoria
        gettimeofday(&t2, NULL); 

        bloqTime += (t2.tv_sec - t1.tv_sec);

        /* ESCRITURA DE MENSAJE EN MEMORIA */ 

        time_t t = time(NULL);
	    struct tm tm = *localtime(&t);
	    char *msg[5] = {"Hola", "TEC", "Empotrados", "SO", "Adios"};

	    buffer[variables[0].tam].pid = getpid();
	    buffer[variables[0].tam].magic_number = rand() % 5;

	    char date[25];
	    sprintf(date, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
	    strcpy(buffer[variables[0].tam].date, date);

	    char hour[25];
	    sprintf(hour, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
	    strcpy(buffer[variables[0].tam].hour, hour);

	    strcpy(buffer[variables[0].tam].text, msg[rand() % 5]);

	    char out[150];
        printc("-> ", 4);
	    sprintf(out, "Escrito un mensaje en indice %d:\n- Productores activos: %d\n- Consumidores activos: %d\n", variables[0].tam, variables[0].productores, variables[0].consumidores);
	    printc(out, 5);

        char time[50];
        sprintf(time, "- Tiempo de espera para produccion: %f\n", timeAleat);
        printc(time, 5);

	    char text2[200];
	    sprintf(text2, "- Mensaje: %s", buffer[variables[0].tam].text);
	    printc(text2, 6);

	    char text1[200];
	    sprintf(text1, " - %s | %s \n\n", buffer[variables[0].tam].date, buffer[variables[0].tam].hour);
	    printc(text1, 6);

	    variables[0].tam ++;
        variables[0].producidos ++;

        if(variables[0].tam == variables[0].sizeMem){
            variables[0].tam = 0; 
        }

        // printc("Terminando proceso: \n", 1);
        // printf("mem: %i\n", sem_get_value(mem, 0));
        // printf("vacio: %i\n", sem_get_value(vacio, 0));
        // printf("lleno: %i\n", sem_get_value(lleno, 0));

        upSem(mem, 0);   //Liberamos zona critica aumentando el semaforo
        upSem(lleno,0);	 //Aumentamos semaforo, indicando que ya hay mensaje dispo para consumir

        
        variables[0].bloqTotal += (t2.tv_sec - t1.tv_sec);
        variables[0].esperaTotal += (t4.tv_sec - t3.tv_sec);

        numMsg ++;
    }

}

// https://stackoverflow.com/questions/34558230/generating-random-numbers-of-exponential-distribution

double expo(double lambda){  //Numeros aleatorios a traves de una distribucion exponencial
    double u;
    srand((unsigned)time(NULL));
    u = rand() / (RAND_MAX + 1.0);
    return -log(1- u) / (lambda);
}


void endProducer(float tiempo, int numMsg, float timeBloq){
	printc("\n Cancelacion recibida desde finalizador \n\n", 1);
	printc("************ Productor finalizado ************\n", 3);
    char out[200];
	sprintf(out, "- Mensajes producidos por este productor: %i\n- Mensajes producidos por todos los productores: %i\n", numMsg, variables[0].producidos);
    printc(out, 5);
    char out2[200];
	sprintf(out2, "- Tiempo en bloqueo por semaforos: %f\n- Tiempo activo del proceso: %f\n\n", timeBloq, tiempo);
	printc(out2,5);
	exit(0);
}
