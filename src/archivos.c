#include "archivos.h"
#include "procesos.h"
#include "hilos.h" 
#include <string.h>
#include <stdio.h>

int procesarArchivoSesion(const char *rutaArchivo) {
    // abrir archivo (si es que existe) haciendo uso de fopen()
    FILE *archivo = fopen(rutaArchivo, "r");
    if (archivo == NULL) {
        printf("Error: El archivo '%s' no existe o no se pudo abrir.\n", rutaArchivo);
        return 0; 
    }
    printf("EL archivo fue abierto con exito. Lectura Iniciada.\n");

    char linea[256];
    DatosSesion registros[100]; // Arreglo para guardar hasta 100 comandos
    int contador = 0;

    // hacemos uso del fgets() para la lectura linea por linea del archivo
    while (fgets(linea, sizeof(linea), archivo) != NULL && contador < 100) {
        linea[strcspn(linea, "\n")] = '\0'; //elimina el salto de linea
        
        // usamos strtok para dividir la linea cada vez que haya un '|'
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
    printf("Lectura Finalizada. Fin de la lectura del archivo.\n");
    
    // usamos fclose para cerrar el archivo correctamente
    fclose(archivo);

    if (contador > 0) {
        ejecutarProcesamientoConcurrente(registros, contador);

        // === CONEXIÓN CON TU PARTE (Parte 2: Procesos) ===
        printf("\n[SISTEMA] Iniciando la creacion de procesos con fork()...\n");
        for (int i = 0; i < contador; i++) {
            // Llamamos a tu función pasando los datos limpios
            ejecutarProceso(registros[i].usuario, registros[i].comando, registros[i].tiempo);
        }
    } else {
        printf("El archivo estaba vacío o el formato era incorrecto.\n");
    }

    return 1; 
}