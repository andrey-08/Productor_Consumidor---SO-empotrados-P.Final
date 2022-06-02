 
#ifndef _UTIL_H
#define _UTIL_H

int checkDir(const char* path);
int makeDir(const char* path);
int isNumber(char *text);
char *concat(const char *s1, const char *s2);
void printc(char *msg, int color);
#endif