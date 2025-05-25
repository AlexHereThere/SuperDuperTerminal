#include<stdio.h>
#include"InComs.h"
#include"Procesadero.h"
#include"memory_manager.h"

int es_com_bash=0;
int es_com_ter=0;

void mostrar_ayuda() {
    printf("\n========== COMANDOS DISPONIBLES ==========\n");
    printf("GESTIÓN DE PROCESOS:\n");
    printf("  creaproceso <burst_time> <block_size>  - Crear proceso manual\n");
    printf("  creaprocesos <archivo.csv>             - Crear procesos desde CSV\n");
    printf("  listaprocesos                          - Listar todos los procesos\n");
    printf("  kill <id>                              - Eliminar proceso\n");
    printf("\nGESTIÓN DE MEMORIA:\n");
    printf("  alloc <id> <estrategia>                - Asignar memoria a proceso\n");
    printf("    Estrategias: first, best, worst\n");
    printf("  free <id>                              - Liberar memoria de proceso\n");
    printf("  mstatus                                - Ver estado de memoria\n");
    printf("  compact                                - Compactar memoria\n");
    printf("\nALGORITMOS DE SCHEDULING:\n");
    printf("  FCFS                                   - First Come First Served\n");
    printf("  SJF                                    - Shortest Job First\n");
    printf("  RR <quantum>                           - Round Robin\n");
    printf("\nOTROS:\n");
    printf("  help                                   - Mostrar esta ayuda\n");
    printf("  exit                                   - Salir del programa\n");
    printf("==========================================\n\n");
}

int main()
{
    nodo_p_t* head = NULL;
    char* comandos;
    
    // Inicializar el sistema de memoria
    inicializar_memoria();
    
    printf("========================================\n");
    printf("    SUPER DUPER TERMINAL v2.0\n");
    printf("  Sistema Operativo con Gestión de Memoria\n");
    printf("========================================\n");
    printf("Memoria inicializada con %d bloques.\n", MEMORY_SIZE);
    printf("Escriba 'help' para ver comandos disponibles.\n\n");
    
    while(1)
    {
        printf("SuperDuperTerminal> ");
        comandos = leer_cadena();

        if (strcmp(comandos, "exit") == 0) {
            printf("Liberando recursos...\n");
            // Liberar memoria de procesos
            if(head != NULL) {
                elimina_procesos(head);
            }
            // Liberar memoria del manager
            liberar_memoria_manager();
            free(comandos);
            break;//salir
        }
        
        if (strcmp(comandos, "help") == 0) {
            mostrar_ayuda();
            free(comandos);
            continue;
        }
        
        // Intentar como comando del sistema
        Interpret_command(comandos);
        if(es_com_bash) {
            free(comandos);
            continue;//si fue comando de bash, brincar lo demas.
        }
        
        // Manejar comandos de procesos y memoria
        manejar_procesos(comandos, &head);
        
        free(comandos);
    }
    
    printf("Adios...\n");
    return 0;
}
