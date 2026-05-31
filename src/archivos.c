#include "archivos.h"
#include <string.h>

int procesarArchivoSesion(const char *rutaArchivo) {
    // abrir archivo (si es que existe) haciendo uso de fopen()
    FILE *archivo = fopen(rutaArchivo, "r");
    if (archivo == NULL) {
        printf("Error: El archivo '%s' no existe o no se pudo abrir.\n", rutaArchivo);
        return 0; 
    }
    printf("EL archivo fue abierto con exito. Lectura Iniciada.\n");

    char linea[256];

    // hacemos uso del fgets() para la lectura linea por linea del archivo
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        linea[strcspn(linea, "\n")] = '\0'; //elimina el salto de linea
        
        // usamos strtok para dividir la linea cada vez que haya un '|'
        char *id_comando = strtok(linea, "|");
        char *usuario    = strtok(NULL, "|");
        char *comando    = strtok(NULL, "|");
        char *tiempo     = strtok(NULL, "|");

        // verificamos que la linea tenga todos los campos requeridos (formato) antes de ser procesada
        if (id_comando != NULL && usuario != NULL && comando != NULL && tiempo != NULL) {
            printf("Comando ID: %s | Usuario: %s | Ejecuta: %s | Tiempo: %s seg\n", 
                   id_comando, usuario, comando, tiempo);
        } else {
            printf("Error: Linea con formato incorrecto o faltante.\n");
        }
    }
    printf("Lectura Finalizada. Fin de la lectura del archivo.\n");

    // usamos fclose para cerrar el archivo correctamente
    fclose(archivo);
    return 1; 
}