#include <stdio.h>
#include <stdlib.h>
#include "archivos.h"
#include "procesos.h"
#include "hilos.h"
#include "generador.h"

int main(int argc, char *argv[]) {
    printf("\nSISTEMA INTEGRADO SHELLDOC WEB - GRUPO 2\n\n");

    // Definición de rutas relativas de trabajo
    const char *ruta_sesion   = "../data/sesion_linux.txt";
    const char *ruta_template = "../quarto/template.qmd";
    const char *ruta_salida   = "../quarto/informacion.qmd";
    
    const char *titulo_doc    = "Reporte Automatizado de Sesión Linux";
    const char *autor_doc     = "Grupo 2 - EPN";

    // 1. Ejecutar Fase 1, 2 y 3 (Lectura, Hilos y tus Procesos Fork)
    printf("Iniciando Procesamiento Base...\n");
    int res_base = procesarArchivoSesion(ruta_sesion);
    if (!res_base) {
        fprintf(stderr, "ERROR: Falla crítica en el procesamiento base.\n");
        return EXIT_FAILURE;
    }

    // 2. Ejecutar Fase 4 (Generador de plantilla .qmd de Juan Jácome)
    printf("\nIniciando Fase 4 (Generación de Documento .qmd)...\n");
    int res_fase4 = generarDocumentoQmd(ruta_sesion, ruta_template, ruta_salida, titulo_doc, autor_doc);
    if (!res_fase4) {
        fprintf(stderr, "ERROR: No se pudo estructurar el archivo Quarto de salida.\n");
        return EXIT_FAILURE;
    }

    // 3. Ejecutar Fase 5 (Tu rol: Automatizar el renderizado a HTML usando system)
    printf("\nIniciando Fase 5 (Renderizado de Quarto a HTML)...\n");
    int resultado_render = system("quarto render ../quarto/informacion.qmd --to html");

    if (resultado_render == 0) {
        printf("\n¡PIPELINE EJECUTADO CON ÉXITO!\n");
        printf("Documento web generado en: ../quarto/informacion.html\n");
    } else {
        fprintf(stderr, "\nEl programa corrió, pero Quarto falló al renderizar.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}