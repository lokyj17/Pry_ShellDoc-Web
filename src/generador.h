#ifndef GENERADOR_H
#define GENERADOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/utsname.h>

/* ============================================================
   ESTRUCTURAS
   ============================================================ */

/* Representa una linea parseada del sesionLinux.txt */
typedef struct {
    char id_comando[16];
    char usuario[64];
    char comando[256];
    char tiempo[16];
} EntradaSesion;

/* Metadatos del documento Quarto */
typedef struct {
    char titulo[256];
    char autor[256];
    char fecha[64];
    char sistema_os[128];
    char usuario_sistema[64];
} MetadatosDoc;

/* ============================================================
   PROTOTIPOS
   ============================================================ */

/*
 * Lee el archivo sesionLinux.txt ya procesado por archivos.c
 * y carga las entradas en el arreglo 'entradas'.
 * Retorna el numero de entradas leidas, -1 si hay error.
 */
int cargarSesion(const char *rutaArchivo, EntradaSesion *entradas, int max_entradas);

/*
 * Obtiene los metadatos del sistema (OS, usuario, fecha)
 * y los carga en la estructura MetadatosDoc.
 */
void obtenerMetadatos(MetadatosDoc *meta, const char *titulo, const char *autor);

/*
 * Lee template.qmd, reemplaza los placeholders {{...}}
 * y escribe el resultado en rutaSalida.
 * Retorna 1 si tuvo exito, 0 si hubo error.
 */
int reemplazarPlaceholders(const char *rutaTemplate,
                           const char *rutaSalida,
                           const MetadatosDoc *meta,
                           const EntradaSesion *entradas,
                           int n_entradas);

/*
 * Escribe un bloque Quarto/Markdown para una EntradaSesion.
 * Usado internamente por reemplazarPlaceholders.
 */
void escribirBloqueComando(FILE *f, const EntradaSesion *entrada);

/*
 * Funcion principal del generador:
 * orquesta cargarSesion + obtenerMetadatos + reemplazarPlaceholders.
 * Retorna 1 si tuvo exito, 0 si hubo error.
 */
int generarDocumentoQmd(const char *rutaSesion,
                        const char *rutaTemplate,
                        const char *rutaSalida,
                        const char *titulo,
                        const char *autor);

#endif /* GENERADOR_H */
