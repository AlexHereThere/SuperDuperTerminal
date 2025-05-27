#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"infinicadenas.h"
#include"dividecadenas.h"
#include"lista_procesos.h"
#include"FCFS.h"
#include"SJF.h"
#include"RR.h"
#include"memory_manager.h"

//declaracion
void manejar_procesos(char* comando, nodo_p_t** head);
void crear_procesos_csv(nodo_p_t** head, char** c_partes);

void manejar_procesos(char* comando, nodo_p_t** head)
{
    int num_com;
    char* str_aux;
    char** c_partes;
    nodo_p_t* aux = NULL;	

    c_partes = tokensToArray(comando, " ");

    // Verificar si c_partes es NULL
    if (!c_partes) {
        printf("Error: no se pudo procesar el comando.\n");
        return;
    }

    // Comandos de gestión de procesos
    if(strcmp(c_partes[0], "creaprocesos") == 0) {
        crear_procesos_csv(head, c_partes);
        freeArray(c_partes);
        return;
    }
    
    if(strcmp(c_partes[0], "listaprocesos") == 0) {
        show_all(*head);
        freeArray(c_partes);
        return;
    }
    
    if(strcmp(c_partes[0], "mykill") == 0) {
        int num_partes = arrayLength(c_partes);
        if(num_partes < 2) {
            printf("Error: no se ingreso el ID del proceso a eliminar.\n");
            freeArray(c_partes);
            return;
        }
        
        char* str_aux;
        unsigned int id_eliminar = strtol(c_partes[1], &str_aux, 10);
        if(c_partes[1] == str_aux || *str_aux != '\0') {
            printf("Error: ID del proceso debe ser un número válido.\n");
            freeArray(c_partes);
            return;
        }
        
        // Verificar si el proceso existe antes de intentar eliminarlo
        nodo_p_t* proceso = buscar_proceso_por_id(*head, id_eliminar);
        if(proceso == NULL) {
            printf("Error: no existe proceso con ID %u.\n", id_eliminar);
            freeArray(c_partes);
            return;
        }
        
        // Si el proceso está en memoria, liberarlo primero
        if(proceso->proc.estado == ready) {
            printf("Liberando proceso P%u de memoria antes de eliminarlo...\n", id_eliminar);
            free_proceso(id_eliminar, head);
        }
        
        // Eliminar el proceso de la lista
        rm(id_eliminar, head);
        printf("Proceso P%u eliminado exitosamente.\n", id_eliminar);
        freeArray(c_partes);
        return;
    }
    
    // Algoritmos de scheduling
    if(strcmp(c_partes[0], "FCFS") == 0) {
        if((*head) == NULL) {
            printf("Error: no se puede aplicar algoritmo FCFS. (lista de procesos vacio)\n");
            freeArray(c_partes);
            return;
        }
        (*head) = ordenar_procesos_ID(*head); //FCFS solamente ocupa ordenarlos por ID.
        aux = (*head); //resguardar
        realizarFCFS(*head);
        elimina_procesos(aux); //liberar memoria de lista
        *head = NULL;
        freeArray(c_partes);
        return;
    } 
    
    if(strcmp(c_partes[0], "SJF") == 0) {
        if((*head) == NULL) {
            printf("Error: no se puede aplicar algoritmo SJF. (lista de procesos vacio)\n");
            freeArray(c_partes);
            return;
        }
        (*head) = ordenar_procesos_SJF(*head); //SJF ordena de una manera particular.
        aux = (*head);
        realizarSJF(*head);
        elimina_procesos(aux);
        (*head) = NULL;
        freeArray(c_partes);
        return;
    }
    
    if(strcmp(c_partes[0], "RR") == 0) {
        if((*head) == NULL) {
            printf("Error: no se puede aplicar algoritmo RR. (lista de procesos vacio)\n");
            freeArray(c_partes);
            return;
        }
        num_com = arrayLength(c_partes);
        if(num_com == 2) { //hay 2 parametros en el comando
            unsigned int qt = strtol(c_partes[1], &str_aux, 10); //se lee un quantum time
            if(qt <= 0) { //se asigno un valor invalido como qt.
                printf("Error: quantum time invalido.\n");
                freeArray(c_partes);
                return;
            }
                
            (*head) = ordenar_procesos_ID(*head);
            aux = (*head);
            realizarRR(*head, qt);
            (*head) = NULL;
            freeArray(c_partes);
            return;
        } else {
            printf("Error: falto quantum time o se agrego un parametro de mas.\n");
            freeArray(c_partes);
            return;
        }
    }
    
    // Comandos de gestión de memoria
    if(strcmp(c_partes[0], "myalloc") == 0) {
        int num_partes = arrayLength(c_partes);
        if(num_partes < 3) {
            printf("Error: uso correcto -> myalloc <id> <estrategia>\n");
            printf("Estrategias disponibles: first, best, worst\n");
            freeArray(c_partes);
            return;
        }
        
        char* str_aux;
        unsigned int proceso_id = strtol(c_partes[1], &str_aux, 10);
        if(c_partes[1] == str_aux || *str_aux != '\0') {
            printf("Error: ID del proceso debe ser un número válido.\n");
            freeArray(c_partes);
            return;
        }
        
        // Buscar el proceso
        nodo_p_t* proceso = buscar_proceso_por_id(*head, proceso_id);
        if(proceso == NULL) {
            printf("Error: no existe proceso con ID %u.\n", proceso_id);
            freeArray(c_partes);
            return;
        }
        
        // Convertir estrategia
        estrategia_t estrategia;
        if(strcmp(c_partes[2], "first") == 0) {
            estrategia = FIRST_FIT;
        } else if(strcmp(c_partes[2], "best") == 0) {
            estrategia = BEST_FIT;
        } else if(strcmp(c_partes[2], "worst") == 0) {
            estrategia = WORST_FIT;
        } else {
            printf("Error: estrategia no válida. Use 'first', 'best' o 'worst'.\n");
            freeArray(c_partes);
            return;
        }
        
        // Asignar memoria
        int resultado = alloc_proceso(proceso_id, proceso->proc.blockSize, estrategia, head);
        if(resultado) {
            printf("Memoria asignada correctamente al proceso P%u.\n", proceso_id);
        } else {
            printf("No se pudo asignar memoria al proceso P%u. Memoria insuficiente o proceso ya en memoria.\n", proceso_id);
        }
        freeArray(c_partes);
        return;
    }
    
    if(strcmp(c_partes[0], "myfree") == 0) {
        int num_partes = arrayLength(c_partes);
        if(num_partes < 2) {
            printf("Error: uso correcto -> myfree <id>\n");
            freeArray(c_partes);
            return;
        }
        
        char* str_aux;
        unsigned int proceso_id = strtol(c_partes[1], &str_aux, 10);
        if(c_partes[1] == str_aux || *str_aux != '\0') {
            printf("Error: ID del proceso debe ser un número válido.\n");
            freeArray(c_partes);
            return;
        }
        
        int resultado = free_proceso(proceso_id, head);
        if(resultado) {
            printf("Memoria liberada correctamente para el proceso P%u.\n", proceso_id);
        } else {
            printf("No se pudo liberar memoria del proceso P%u. Verifique que exista y esté en memoria.\n", proceso_id);
        }
        freeArray(c_partes);
        return;
    }
    
    if(strcmp(c_partes[0], "mystatus") == 0) {
        mostrar_memoria_status();
        freeArray(c_partes);
        return;
    }
    
    if(strcmp(c_partes[0], "mycompact") == 0) {
        compactar_memoria(head);
        printf("Memoria compactada exitosamente.\n");
        freeArray(c_partes);
        return;
    }
    
    // Si llegamos aquí, el comando no fue reconocido
    printf("Comando no reconocido: %s\n", c_partes[0]);
    printf("Escriba 'help' para ver los comandos disponibles.\n");
    freeArray(c_partes);
}

void crear_procesos_csv(nodo_p_t** head, char** c_partes)
{
    char* aux;
    char line[256];
    int num_partes = arrayLength(c_partes);
    int procesos_creados = 0;
    
    if(num_partes < 2) {
        printf("Error: no se ingreso el csv con los procesos.\n");
        return;
    }
    
    FILE* file = fopen(c_partes[1], "r");
    if(file == NULL) {
        printf("Error: archivo no existe o hubo problema.\n");
        return;
    }
    
    while(fgets(line, sizeof(line), file)) {
        // Eliminar el salto de línea si existe
        size_t len = strlen(line);
        if (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[len-1] = '\0';
        }
        
        char** aux_partes = tokensToArray(line, ",");
        if(arrayLength(aux_partes) < 2) {
            printf("Error: Línea con formato incorrecto en el CSV.\n");
            freeArray(aux_partes);
            continue; // Continuar con la siguiente línea en lugar de abortar
        }
        
        unsigned int bt_value = strtol(aux_partes[0], &aux, 10);
        if(bt_value <= 0) {
            printf("Error: burst time inválido en línea del CSV.\n");
            freeArray(aux_partes);
            continue;
        }
        
        unsigned int bz_value = strtol(aux_partes[1], &aux, 10);
        if(bz_value <= 0) {
            printf("Error: block size inválido en línea del CSV.\n");
            freeArray(aux_partes);
            continue;
        }
        
        proceso_t new_proceso;
        new_proceso.id = buscar_id_disponible(*head);
        new_proceso.burstTime = bt_value;
        new_proceso.currentBT = bt_value;
        new_proceso.blockSize = bz_value;
        new_proceso.estado = p_new;
        new_proceso.waitingTime = 0;
        
        in(new_proceso, head);
        procesos_creados++;
        
        freeArray(aux_partes);
    }
    
    fclose(file);
    printf("Procesos del CSV cargados exitosamente. Se crearon %d procesos.\n", procesos_creados);
}