#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

/* VERIFICA EXISTENCIA DE CARPETA */
int checkDir(const char* path){
  struct stat sb;
  if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode)) {
    return 1;
  }else {
    return 0;
  }   
}

/* CREAR CARPETA O DIRECTORIO */
int makeDir(const char* path){
  if(mkdir(path, 0700) == -1){
    return 0;
  }
  return 1;
}

/* COMPROBAR ARGUMENTOS NUMERICOS. */
int isNumber(char *text){
  int j;
  j = strlen(text);
  while(j--){
    if(text[j] > 47 && text[j] < 58)
      continue;
    return 0;
  }
  return 1;
}


/* CONCATENAR 2 STRINGS */
char *concat(const char *s1, const char *s2){
  char *result = (char *)malloc(strlen(s1) + strlen(s2) + 1);
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}

/**
 * IMPRIMIR TEXTO A COLOR
 * 1: Bold Red
 * 2: Bold Blue
 * 3: Bold Green
 * 4: Bold Yellow
 * 5: Bold Magenta
 * 6: Bold Cyan
*/
void printc(char *msg, int color){
  if (color == 1){
    printf("\033[1;31m");
    printf("%s",msg);
    printf("\033[0m");
  } else if (color == 2){
    printf("\033[1;34m");
    printf("%s",msg);
    printf("\033[0m");
  } else if (color == 3){
    printf("\033[1;32m");
    printf("%s",msg);
    printf("\033[0m");
  }else if (color == 4){
    printf("\033[1;33m");
    printf("%s",msg);
    printf("\033[0m");
  }else if (color == 5){
    printf("\033[1;35m");
    printf("%s",msg);
    printf("\033[0m");
  }else if (color == 6){
    printf("\033[1;36m");
    printf("%s",msg);
    printf("\033[0m");
  }
}