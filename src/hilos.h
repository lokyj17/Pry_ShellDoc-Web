#ifndef HILOS_H
#define HILOS_H

#include <pthread.h>

// Estructura para empaquetar la información de una línea leída
typedef struct {
    char id_comando[20];
    char usuario[50];
    char comando[256];
    char tiempo[20];
    // Aquí el hilo guardará el texto ya formateado y listo para el archivo .qmd
    char resultado_markdown[512]; 
} DatosSesion;

// Funciones que implementaremos
void* procesarRegistro(void* arg);
void ejecutarProcesamientoConcurrente(DatosSesion* arregloDatos, int cantidadRegistros);

#endif