#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "capturador.h"   
#include "archivos.h"
#include "procesos.h"
#include "hilos.h"
#include "generador.h"

int main(int argc, char *argv[]) {
    printf("\nSISTEMA INTEGRADO SHELLDOC WEB - GRUPO 2\n\n");

    char ruta_sesion[512];

    if (argc < 2) {
        printf("Fase 0: Iniciando captura interactiva de sesión...\n");
        int capturados = ejecutarSesion();

        if (capturados == 0) {
            fprintf(stderr, "ERROR: No se capturó ningún comando. Abortando.\n");
            return EXIT_FAILURE;
        }

        snprintf(ruta_sesion, sizeof(ruta_sesion), "../data/sesionlinux.txt");

    } else {
        snprintf(ruta_sesion, sizeof(ruta_sesion), "../data/%s", argv[1]);
        printf("Usando archivo de sesión existente: %s\n\n", ruta_sesion);
    }

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
        printf("Documento web generado en: ../docs/index.html\n");
        
        system("explorer.exe $(wslpath -w ../docs/index.html)"); 
    } else {
        fprintf(stderr, "\nEl programa corrió, pero Quarto falló al renderizar.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
