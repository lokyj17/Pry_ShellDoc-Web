#include "procesos.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void ejecutarProceso(const char *usuario, const char *comando,const char *tiempo) {
    pid_t pid = fork();

    if (pid < 0)
    {
        printf("Error al crear el proceso hijo.\n");
        return;
    }

    if (pid == 0)
    {
        printf("--PROCESO HIJO--\n");
        printf("PID del hijo : %d\n", getpid());
        printf("PPID del hijo: %d\n", getppid());
        printf("Usuario : %s\n", usuario);
        printf("Comando : %s\n", comando);
        printf("Tiempo  : %s segundos\n", tiempo);
        printf("Procesamiento completado.\n");
        exit(0);
    }
    else
    {
        wait(NULL);
        printf("--PROCESO PADRE--\n");
        printf("PID del padre: %d\n", getpid());
        printf("Proceso hijo finalizado correctamente.\n");
    }
}