#include "capturador.h"

/* ============================================================
   iniciarSesion()
   Muestra el banner de bienvenida al usuario.
   ============================================================ */
void iniciarSesion(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════════╗\n");
    printf("║        SHELLDOC WEB — Capturador             ║\n");
    printf("║        Grupo 2 - EPN                         ║\n");
    printf("╠══════════════════════════════════════════════╣\n");
    printf("║  Escribe comandos Linux normalmente.         ║\n");
    printf("║  Escribe  exit  para terminar y guardar.     ║\n");
    printf("╚══════════════════════════════════════════════╝\n\n");
}

/* ============================================================
   capturarComando()
   Lee un comando del usuario, lo procesa, lo ejecuta usando 
   fork() + execvp() midiendo su tiempo, y llena el registro.
   ============================================================ */
int capturarComando(RegistroComando *reg, int id, const char *usuario) {
    char buffer[MAX_CMD];

    /* Mostrar el prompt al estilo de una shell real */
    printf("[shelldoc:%s]$ ", usuario);
    fflush(stdout);

    /* Capturar la línea de entrada; si hay error o EOF se termina */
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return 0;
    }

    /* Remover el salto de línea residual (\n) */
    buffer[strcspn(buffer, "\n")] = '\0';

    /* Si la línea está vacía, continuar el bucle sin guardar registro */
    if (strlen(buffer) == 0) {
        return 1;
    }

    /* Si el usuario digita "exit", se procede a cerrar la sesión */
    if (strcmp(buffer, "exit") == 0) {
        return 0;
    }

    /* Rellenar los metadatos iniciales del registro */
    reg->id = id;
    strncpy(reg->usuario, usuario, sizeof(reg->usuario) - 1);
    reg->usuario[sizeof(reg->usuario) - 1] = '\0';
    strncpy(reg->comando, buffer, sizeof(reg->comando) - 1);
    reg->comando[sizeof(reg->comando) - 1] = '\0';

    /* Tokenizar el comando para poder pasarlo a execvp */
    char copia[MAX_CMD];
    strncpy(copia, buffer, sizeof(copia) - 1);
    copia[sizeof(copia) - 1] = '\0';

    char *argv_exec[128];
    int argc_exec = 0;
    char *token = strtok(copia, " \t");
    
    while (token != NULL && argc_exec < 127) {
        argv_exec[argc_exec++] = token;
        token = strtok(NULL, " \t");
    }
    argv_exec[argc_exec] = NULL;

    /* Estructuras para medir el tiempo exacto de ejecución */
    struct timeval t_inicio, t_fin;
    gettimeofday(&t_inicio, NULL);

    /* Creación del proceso hijo mediante Fork */
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        reg->tiempo_seg = 0.0;
        return 1;
    }

    if (pid == 0) {
        /* ── PROCESO HIJO ── */
        execvp(argv_exec[0], argv_exec);
        /* Si execvp retorna, significa que el comando falló o no existe */
        fprintf(stderr, "shelldoc: comando no encontrado: %s\n", argv_exec[0]);
        exit(EXIT_FAILURE);
    }

    /* ── PROCESO PADRE ── */
    int estado;
    waitpid(pid, &estado, 0);
    gettimeofday(&t_fin, NULL);

    /* Cálculo del tiempo transcurrido en segundos */
    double segundos = (t_fin.tv_sec - t_inicio.tv_sec) +
                      (t_fin.tv_usec - t_inicio.tv_usec) / 1e6;
    reg->tiempo_seg = segundos;

    printf("\n[shelldoc] Tiempo de ejecución: %.3f seg\n\n", segundos);
    return 1;
}

/* ============================================================
   guardarSesion()
   Guarda de forma estructurada los comandos en RUTA_SALIDA
   empleando el delimitador '|'.
   ============================================================ */
int guardarSesion(RegistroComando *registros, int total) {
    FILE *f = fopen(RUTA_SALIDA, "w");
    if (f == NULL) {
        fprintf(stderr, "Error: no se pudo crear el archivo '%s'\n", RUTA_SALIDA);
        perror("fopen");
        return 0;
    }

    for (int i = 0; i < total; i++) {
        /* Se guarda con el formato id|usuario|comando|tiempo requerido por el analizador */
        fprintf(f, "%d|%s|%s|%.0f\n",
                registros[i].id,
                registros[i].usuario,
                registros[i].comando,
                registros[i].tiempo_seg < 1.0 ? 1.0 : registros[i].tiempo_seg);
    }

    fclose(f);
    printf("[OK] Sesión guardada de forma exitosa en '%s' (%d comandos).\n", RUTA_SALIDA, total);
    return 1;
}

/* ============================================================
   ejecutarSesion()
   Mapea el bucle principal que controla la shell interactiva.
   ============================================================ */
int ejecutarSesion(void) {
    /* Intentar recuperar la variable de entorno USER */
    char *env_user = getenv("USER");
    char usuario[64];
    if (env_user != NULL) {
        strncpy(usuario, env_user, sizeof(usuario) - 1);
    } else {
        strncpy(usuario, "usuario", sizeof(usuario) - 1);
    }
    usuario[sizeof(usuario) - 1] = '\0';

    iniciarSesion();

    RegistroComando registros[MAX_COMANDOS];
    int total = 0;  
    int id_actual = 1; 

    while (id_actual <= MAX_COMANDOS) {
        RegistroComando reg;
        memset(&reg, 0, sizeof(reg));

        int resultado = capturarComando(&reg, id_actual, usuario);

        if (resultado == 0) {
            break; /* El usuario solicitó salir con "exit" */
        }

        if (reg.id > 0) {
            registros[total] = reg;
            total++;
            id_actual++;
        }
    }

    if (total == 0) {
        printf("\n[shelldoc] No se capturó ningún comando. Archivo omitido.\n\n");
        return 0;
    }

    printf("\n[shelldoc] Sesión finalizada por el usuario. Almacenando...\n");
    guardarSesion(registros, total);
    return total;
}