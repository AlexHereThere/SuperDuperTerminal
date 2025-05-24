#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "infinicadenas.h"
#include "dividecadenas.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define READ_END 0
#define WRITE_END 1
#define MAX_ESCAPED 450
#define MAX_COMMAND 512

extern int es_com_bash;

void Interpret_command(char* commands)
{
    es_com_bash = 1; // asumimos éxito al inicio

    char** c_partes;
    char** args;
    int num_coms, num_args, num_pipes;
    pid_t son_pid;

    c_partes = tokensToArray(commands, "|"); // dividir en comandos por pipe
    num_coms = arrayLength(c_partes);

    if (num_coms == 0) 
    {
        freeArray(c_partes);
        return; // sin comandos
    }

    num_pipes = num_coms - 1;

    if (num_pipes > 0)
    {
        int pipes[num_pipes][2];
        int i, j;
        pid_t pids[num_coms];

        // Crear pipes
        for (i = 0; i < num_pipes; i++) pipe(pipes[i]);

        // Ejecutar cada comando
        for (i = 0; i < num_coms; i++)
        {
            pids[i] = fork();
            if (pids[i] == 0)
            {
                // Redirección
                if (i == 0) // primer comando
                {
                    dup2(pipes[i][WRITE_END], STDOUT_FILENO);
                }
                else if (i == num_coms - 1) // último comando
                {
                    dup2(pipes[i - 1][READ_END], STDIN_FILENO);
                }
                else // comando intermedio
                {
                    dup2(pipes[i - 1][READ_END], STDIN_FILENO);
                    dup2(pipes[i][WRITE_END], STDOUT_FILENO);
                }

                // Cerrar todos los pipes
                for (j = 0; j < num_pipes; j++)
                {
                    close(pipes[j][READ_END]);
                    close(pipes[j][WRITE_END]);
                }

                args = tokensToArray(c_partes[i], " ");
                execvp(args[0], args);
                // Si exec falla
                freeArray(args);
                exit(EXIT_FAILURE);
            }
        }

        // Padre cierra pipes
        for (i = 0; i < num_pipes; i++)
        {
            close(pipes[i][READ_END]);
            close(pipes[i][WRITE_END]);
        }

        // Esperar a todos los hijos y revisar estado
        for (i = 0; i < num_coms; i++)
        {
            int status;
            waitpid(pids[i], &status, 0);
            if (!(WIFEXITED(status) && WEXITSTATUS(status) == 0)) es_com_bash = 0; // falló algún hijo
        }

        freeArray(c_partes);
    }
    else // sin pipes
    {
        son_pid = fork();
        if (son_pid == 0)
        {
            args = tokensToArray(c_partes[0], " ");
            execvp(args[0], args);
            freeArray(args);
            exit(EXIT_FAILURE);
        }
        else
        {
            int status;
            waitpid(son_pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) es_com_bash = 1;
            else es_com_bash = 0;
        }
        freeArray(c_partes);
    }
}