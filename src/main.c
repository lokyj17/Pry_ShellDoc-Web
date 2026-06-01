#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "archivos.h"
#include "procesos.h"
#include "hilos.h"
#include "generador.h"

int main(int argc, char *argv[]) {
    printf("\nSISTEMA INTEGRADO SHELLDOC WEB - GRUPO 2\n\n");

    if (argc < 2) {
        fprintf(stderr, "ERROR: Debes especificar el nombre del archivo ubicado en data.\n");
        fprintf(stderr, "Uso correcto: %s <nombre_archivo.txt>\n", argv[0]);
        fprintf(stderr, "Ejemplo: %s sesion_linux.txt\n\n", argv[0]);
        return EXIT_FAILURE;
    }

    char ruta_sesion[512];
    snprintf(ruta_sesion, sizeof(ruta_sesion), "../data/%s", argv[1]);

    const char *ruta_template = "../quarto/template.qmd";
    const char *ruta_salida   = "../quarto/informacion.qmd";
    
    const char *titulo_doc    = "Reporte Automatizado de Sesión Linux";
    const char *autor_doc     = "Grupo 2 - EPN";

    printf("\nLeyendo archivo desde: %s\n\n", ruta_sesion);

    printf("Iniciando Procesamiento Base...\n");
    printf("\nPID: %d, PPID: %d - Coordinando hilos y forks...\n", getpid(), getppid());
    
    int res_base = procesarArchivoSesion(ruta_sesion);
    if (!res_base) {
        fprintf(stderr, "ERROR: Falla crítica en el procesamiento base.\n");
        return EXIT_FAILURE;
    }

    printf("\nIniciando Fase 4 (Generación de Documento .qmd)...\n");
    int res_fase4 = generarDocumentoQmd(ruta_sesion, ruta_template, ruta_salida, titulo_doc, autor_doc);
    if (!res_fase4) {
        fprintf(stderr, "ERROR: No se pudo estructurar el archivo Quarto de salida.\n");
        return EXIT_FAILURE;
    }





    printf("\nIniciando Fase 5 (Renderizado de Quarto a HTML)...\n");
    int resultado_render = system("quarto render ../quarto/informacion.qmd --to html --embed-resources && mv ../quarto/informacion.html ../docs/index.html");

    if (resultado_render == 0) {
        printf("\n¡EJECUTADO CON ÉXITO!\n");
        printf("Documento web generado en: ../docs/index.html\n"); // Corregimos la mentira
        
        // Traducimos la ruta para que Windows no se pierda
        system("explorer.exe $(wslpath -w ../docs/index.html)"); 
    } else {
        fprintf(stderr, "\nEl programa corrió, pero Quarto falló al renderizar.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}