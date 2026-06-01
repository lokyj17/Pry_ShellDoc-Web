#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hilos.h"

// Esta es la "rutina" que ejecutará cada hilo de forma paralela
void* procesarRegistro(void* arg) {
    // Transformamos el puntero genérico a nuestra estructura
    DatosSesion* datos = (DatosSesion*)arg;

    // Tarea de normalización: Convertir el nombre de usuario a MAYÚSCULAS 
    // (Ejemplo de limpieza/procesamiento de datos)
    for(int i = 0; datos->usuario[i]; i++){
        datos->usuario[i] = toupper(datos->usuario[i]);
    }

    // Tarea de transformación: Armar el bloque de texto en formato Markdown
    // Esto es lo que el paso 4 escribirá en el archivo para que Quarto lo entienda.
    snprintf(datos->resultado_markdown, sizeof(datos->resultado_markdown), 
             "### Ejecución: %s\n- **Usuario:** %s\n- **Tiempo:** %s seg\n\n```bash\n%s\n```\n---\n",
             datos->id_comando, datos->usuario, datos->tiempo, datos->comando);

    // El hilo termina su trabajo
    pthread_exit(NULL);
}

// Función orquestadora que recibe un arreglo de registros leídos
void ejecutarProcesamientoConcurrente(DatosSesion* arregloDatos, int cantidadRegistros) {
    // Declaramos un arreglo de identificadores de hilos
    pthread_t hilos[cantidadRegistros];

    printf("Iniciando procesamiento concurrente con %d hilos...\n", cantidadRegistros);

    // 1. Crear un hilo por cada registro de la sesión Linux
    for (int i = 0; i < cantidadRegistros; i++) {
        // A cada hilo le pasamos la dirección de memoria de su registro específico
        if (pthread_create(&hilos[i], NULL, procesarRegistro, (void*)&arregloDatos[i]) != 0) {
            printf("Error al crear el hilo para el registro %d\n", i);
        }
    }

    // 2. Sincronización: Esperar a que todos los hilos terminen su trabajo
    for (int i = 0; i < cantidadRegistros; i++) {
        pthread_join(hilos[i], NULL);
        
        // Verificamos en consola el trabajo que hizo cada hilo
        printf("Hilo %d terminó. Resultado:\n%s", i, arregloDatos[i].resultado_markdown);
    }
    
    printf("Procesamiento concurrente finalizado exitosamente.\n");
}