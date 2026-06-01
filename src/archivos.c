#include "archivos.h"
#include "hilos.h" // Importante: incluimos tu cabecera de hilos
#include <string.h>
#include <stdio.h>

int procesarArchivoSesion(const char *rutaArchivo) {
    FILE *archivo = fopen(rutaArchivo, "r");
    if (archivo == NULL) {
        printf("Error: El archivo '%s' no se pudo abrir.\n", rutaArchivo);
        return 0; 
    }

    char linea[256];
    DatosSesion registros[100]; // Arreglo para guardar hasta 100 comandos
    int contador = 0;

    // Leemos el archivo y llenamos nuestro arreglo
    while (fgets(linea, sizeof(linea), archivo) != NULL && contador < 100) {
        linea[strcspn(linea, "\n")] = '\0'; 
        
        char *id_comando = strtok(linea, "|");
        char *usuario    = strtok(NULL, "|");
        char *comando    = strtok(NULL, "|");
        char *tiempo     = strtok(NULL, "|");

        if (id_comando && usuario && comando && tiempo) {
            strcpy(registros[contador].id_comando, id_comando);
            strcpy(registros[contador].usuario, usuario);
            strcpy(registros[contador].comando, comando);
            strcpy(registros[contador].tiempo, tiempo);
            contador++;
        }
    }
    fclose(archivo);

    // Aquí está la conexión: Llamamos a tu módulo de concurrencia
    if (contador > 0) {
        ejecutarProcesamientoConcurrente(registros, contador);
    } else {
        printf("El archivo estaba vacío o el formato era incorrecto.\n");
    }

    return 1; 
}