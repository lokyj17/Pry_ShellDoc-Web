#include "generador.h"
#include "archivos.h"

/* Maximo de comandos soportados por sesion */
#define MAX_ENTRADAS 128
#define MAX_BUFFER   4096

/* ============================================================
   cargarSesion()
   Lee sesionLinux.txt con el mismo formato que archivos.c:
   id|usuario|comando|tiempo
   ============================================================ */
int cargarSesion(const char *rutaArchivo, EntradaSesion *entradas, int max_entradas) {
    FILE *archivo = fopen(rutaArchivo, "r");
    if (archivo == NULL) {
        printf("Error: No se pudo abrir '%s'\n", rutaArchivo);
        return -1;
    }

    int count = 0;
    char linea[512];

    while (fgets(linea, sizeof(linea), archivo) != NULL && count < max_entradas) {
        /* Eliminar salto de linea igual que en archivos.c */
        linea[strcspn(linea, "\n")] = '\0';

        /* Parsear con strtok igual que en archivos.c */
        char *id_comando = strtok(linea, "|");
        char *usuario    = strtok(NULL, "|");
        char *comando    = strtok(NULL, "|");
        char *tiempo     = strtok(NULL, "|");

        /* Solo procesar lineas con formato correcto */
        if (id_comando != NULL && usuario != NULL && comando != NULL && tiempo != NULL) {
            strncpy(entradas[count].id_comando, id_comando, sizeof(entradas[count].id_comando) - 1);
            strncpy(entradas[count].usuario,    usuario,    sizeof(entradas[count].usuario)    - 1);
            strncpy(entradas[count].comando,    comando,    sizeof(entradas[count].comando)    - 1);
            strncpy(entradas[count].tiempo,     tiempo,     sizeof(entradas[count].tiempo)     - 1);

            /* Asegurar terminacion nula */
            entradas[count].id_comando[sizeof(entradas[count].id_comando) - 1] = '\0';
            entradas[count].usuario   [sizeof(entradas[count].usuario)    - 1] = '\0';
            entradas[count].comando   [sizeof(entradas[count].comando)    - 1] = '\0';
            entradas[count].tiempo    [sizeof(entradas[count].tiempo)     - 1] = '\0';

            count++;
        } else {
            printf("Advertencia: Linea con formato incorrecto omitida.\n");
        }
    }

    fclose(archivo);
    printf("[OK] %d comandos cargados desde '%s'\n", count, rutaArchivo);
    return count;
}

/* ============================================================
   obtenerMetadatos()
   Obtiene fecha, OS y usuario del sistema automaticamente
   ============================================================ */
void obtenerMetadatos(MetadatosDoc *meta, const char *titulo, const char *autor) {
    /* Titulo y autor vienen como parametros */
    strncpy(meta->titulo, titulo, sizeof(meta->titulo) - 1);
    strncpy(meta->autor,  autor,  sizeof(meta->autor)  - 1);

    /* Fecha actual con strftime */
    time_t ahora = time(NULL);
    strftime(meta->fecha, sizeof(meta->fecha), "%d/%m/%Y %H:%M:%S", localtime(&ahora));

    /* Sistema operativo con uname() */
    struct utsname info_os;
    if (uname(&info_os) == 0) {
        snprintf(meta->sistema_os, sizeof(meta->sistema_os),
                 "%s %s %s", info_os.sysname, info_os.release, info_os.machine);
    } else {
        strncpy(meta->sistema_os, "Desconocido", sizeof(meta->sistema_os) - 1);
    }

    /* Usuario del sistema con getenv() */
    char *env_user = getenv("USER");
    if (env_user != NULL) {
        strncpy(meta->usuario_sistema, env_user, sizeof(meta->usuario_sistema) - 1);
    } else {
        strncpy(meta->usuario_sistema, "desconocido", sizeof(meta->usuario_sistema) - 1);
    }
}

/* ============================================================
   escribirBloqueComando()
   Escribe un bloque Markdown/Quarto para una entrada
   ============================================================ */
void escribirBloqueComando(FILE *f, const EntradaSesion *entrada) {
    fprintf(f, "### Comando #%s\n\n", entrada->id_comando);
    fprintf(f, "| Campo    | Valor |\n");
    fprintf(f, "|----------|-------|\n");
    fprintf(f, "| Usuario  | `%s` |\n", entrada->usuario);
    fprintf(f, "| Tiempo   | %s seg |\n\n", entrada->tiempo);
    fprintf(f, "```bash\n%s\n```\n\n", entrada->comando);
    fprintf(f, "---\n\n");
}

/* ============================================================
   reemplazarPlaceholders()
   Lee template.qmd linea por linea y reemplaza {{PLACEHOLDERS}}
   ============================================================ */
int reemplazarPlaceholders(const char *rutaTemplate,
                           const char *rutaSalida,
                           const MetadatosDoc *meta,
                           const EntradaSesion *entradas,
                           int n_entradas) {

    FILE *ft = fopen(rutaTemplate, "r");
    if (ft == NULL) {
        printf("Error: No se pudo abrir template '%s'\n", rutaTemplate);
        return 0;
    }

    FILE *fs = fopen(rutaSalida, "w");
    if (fs == NULL) {
        printf("Error: No se pudo crear '%s'\n", rutaSalida);
        fclose(ft);
        return 0;
    }

    char linea[1024];

    while (fgets(linea, sizeof(linea), ft) != NULL) {

        /* Reemplazar cada placeholder con su valor real */
        if (strstr(linea, "{{TITULO}}")) {
            /* Reemplazar {{TITULO}} */
            char temp[1024];
            char *pos = strstr(linea, "{{TITULO}}");
            int antes = (int)(pos - linea);
            strncpy(temp, linea, antes);
            temp[antes] = '\0';
            strcat(temp, meta->titulo);
            strcat(temp, pos + strlen("{{TITULO}}"));
            fputs(temp, fs);

        } else if (strstr(linea, "{{AUTOR}}")) {
            char temp[1024];
            char *pos = strstr(linea, "{{AUTOR}}");
            int antes = (int)(pos - linea);
            strncpy(temp, linea, antes);
            temp[antes] = '\0';
            strcat(temp, meta->autor);
            strcat(temp, pos + strlen("{{AUTOR}}"));
            fputs(temp, fs);

        } else if (strstr(linea, "{{FECHA}}")) {
            char temp[1024];
            char *pos = strstr(linea, "{{FECHA}}");
            int antes = (int)(pos - linea);
            strncpy(temp, linea, antes);
            temp[antes] = '\0';
            strcat(temp, meta->fecha);
            strcat(temp, pos + strlen("{{FECHA}}"));
            fputs(temp, fs);

        } else if (strstr(linea, "{{SISTEMA_OS}}")) {
            char temp[1024];
            char *pos = strstr(linea, "{{SISTEMA_OS}}");
            int antes = (int)(pos - linea);
            strncpy(temp, linea, antes);
            temp[antes] = '\0';
            strcat(temp, meta->sistema_os);
            strcat(temp, pos + strlen("{{SISTEMA_OS}}"));
            fputs(temp, fs);

        } else if (strstr(linea, "{{USUARIO}}")) {
            char temp[1024];
            char *pos = strstr(linea, "{{USUARIO}}");
            int antes = (int)(pos - linea);
            strncpy(temp, linea, antes);
            temp[antes] = '\0';
            strcat(temp, meta->usuario_sistema);
            strcat(temp, pos + strlen("{{USUARIO}}"));
            fputs(temp, fs);

        } else if (strstr(linea, "{{TOTAL_COMANDOS}}")) {
            char total[16];
            snprintf(total, sizeof(total), "%d", n_entradas);
            char temp[1024];
            char *pos = strstr(linea, "{{TOTAL_COMANDOS}}");
            int antes = (int)(pos - linea);
            strncpy(temp, linea, antes);
            temp[antes] = '\0';
            strcat(temp, total);
            strcat(temp, pos + strlen("{{TOTAL_COMANDOS}}"));
            fputs(temp, fs);

        } else if (strstr(linea, "{{COMANDOS}}")) {
            /* Placeholder especial: escribe TODOS los bloques de comandos */
            for (int i = 0; i < n_entradas; i++) {
                escribirBloqueComando(fs, &entradas[i]);
            }

        } else {
            /* Linea sin placeholder: copiar tal cual */
            fputs(linea, fs);
        }
    }

    fclose(ft);
    fclose(fs);
    printf("[OK] Archivo '%s' generado correctamente.\n", rutaSalida);
    return 1;
}

/* ============================================================
   generarDocumentoQmd()
   Funcion principal: orquesta todo el proceso
   ============================================================ */
int generarDocumentoQmd(const char *rutaSesion,
                        const char *rutaTemplate,
                        const char *rutaSalida,
                        const char *titulo,
                        const char *autor) {

    printf("=== GENERADOR QMD ===\n");

    /* 1. Cargar sesion desde sesionLinux.txt */
    EntradaSesion entradas[MAX_ENTRADAS];
    int n = cargarSesion(rutaSesion, entradas, MAX_ENTRADAS);
    if (n <= 0) {
        printf("Error: No se pudieron cargar entradas de la sesion.\n");
        return 0;
    }

    /* 2. Obtener metadatos del sistema */
    MetadatosDoc meta;
    memset(&meta, 0, sizeof(meta));
    obtenerMetadatos(&meta, titulo, autor);
    printf("[OK] Metadatos obtenidos: %s | %s\n", meta.sistema_os, meta.fecha);

    /* 3. Reemplazar placeholders y generar informacion.qmd */
    int resultado = reemplazarPlaceholders(rutaTemplate, rutaSalida, &meta, entradas, n);

    if (resultado) {
        printf("=== DOCUMENTO GENERADO: %s ===\n", rutaSalida);
    } else {
        printf("=== ERROR AL GENERAR EL DOCUMENTO ===\n");
    }

    return resultado;
}
