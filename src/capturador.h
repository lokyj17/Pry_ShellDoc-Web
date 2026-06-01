#ifndef CAPTURADOR_H
#define CAPTURADOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#define MAX_COMANDOS 256
#define MAX_CMD 512
#define RUTA_SALIDA "../data/sesionlinux.txt"

typedef struct {
    int    id;               
    char   usuario[64];      
    char   comando[MAX_CMD]; 
    double tiempo_seg;       
} RegistroComando;

void iniciarSesion(void);
int capturarComando(RegistroComando *reg, int id, const char *usuario);
int guardarSesion(RegistroComando *registros, int total);
int ejecutarSesion(void);

#endif