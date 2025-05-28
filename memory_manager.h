#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista_procesos.h"

#define MEMORY_SIZE 100  // Tamaño total de memoria en bloques
#define BLOCK_FREE 0      // Bloque libre
#define BLOCK_OCCUPIED 1  // Bloque ocupado

// Estrategias de asignación
typedef enum {
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT
} estrategia_t;

// Estructura para representar un bloque de memoria
typedef struct bloque_memoria {
    unsigned int inicio;        // Posición de inicio del bloque
    unsigned int tamaño;       // Tamaño del bloque
    unsigned int proceso_id;   // ID del proceso (0 si está libre)
    int ocupado;              // 1 si está ocupado, 0 si está libre
    struct bloque_memoria* sig;
} bloque_memoria_t;

// Variables globales
bloque_memoria_t* memoria_head = NULL;
int memoria[MEMORY_SIZE];  // Array para simular la memoria física

// Funciones principales
void inicializar_memoria();
int alloc_proceso(unsigned int proceso_id, unsigned int tamaño, estrategia_t estrategia, nodo_p_t** head);
int free_proceso(unsigned int proceso_id, nodo_p_t** head);
void mostrar_memoria_status();
void desfragmentar__memoria(nodo_p_t** head);

// Funciones auxiliares
bloque_memoria_t* encontrar_bloque_libre(unsigned int tamaño, estrategia_t estrategia);
void insertar_bloque(unsigned int inicio, unsigned int tamaño, unsigned int proceso_id, int ocupado);
void eliminar_bloque(bloque_memoria_t* bloque);
void compactar_bloques_libres();
nodo_p_t* buscar_proceso_por_id(nodo_p_t* head, unsigned int id);
void actualizar_memoria_fisica();

// Implementación de funciones

void inicializar_memoria() {//bien
    // Limpiar memoria física
    for(int i = 0; i < MEMORY_SIZE; i++) {
        memoria[i] = BLOCK_FREE;
    }
    
    // Crear bloque inicial libre
    memoria_head = (bloque_memoria_t*)malloc(sizeof(bloque_memoria_t));
    memoria_head->inicio = 0;
    memoria_head->tamaño = MEMORY_SIZE;
    memoria_head->proceso_id = 0;
    memoria_head->ocupado = 0;
    memoria_head->sig = NULL;
    
    printf("Memoria inicializada con %d bloques.\n", MEMORY_SIZE);
}

int alloc_proceso(unsigned int proceso_id, unsigned int tamaño, estrategia_t estrategia, nodo_p_t** head) { //parece bien
    // Buscar el proceso en la lista
    nodo_p_t* proceso = buscar_proceso_por_id(*head, proceso_id);
    if(proceso == NULL) {
        printf("Error: Proceso P%d no encontrado.\n", proceso_id);
        return 0;
    }
    
    // Verificar que el proceso esté en estado new
    if(proceso->proc.estado != p_new) {
        printf("Error: Proceso P%d no está en estado NEW.\n", proceso_id);
        return 0;
    }
    
    // Verificar que el tamaño solicitado coincida con el blockSize del proceso
    if(tamaño != proceso->proc.blockSize) {
        printf("Error: Tamaño solicitado (%d) no coincide con blockSize del proceso (%d).\n", 
               tamaño, proceso->proc.blockSize);
        return 0;
    }
    
    // Buscar bloque libre según estrategia
    bloque_memoria_t* bloque_libre = encontrar_bloque_libre(tamaño, estrategia);
    if(bloque_libre == NULL) {
        printf("Error: No hay espacio suficiente en memoria para proceso P%d (tamaño: %d).\n", 
               proceso_id, tamaño);
        return 0;
    }
    
    // Asignar memoria
    unsigned int inicio = bloque_libre->inicio;
    
    // Si el bloque es exactamente del tamaño necesario
    if(bloque_libre->tamaño == tamaño) {
        bloque_libre->proceso_id = proceso_id;
        bloque_libre->ocupado = 1;
    } 
    // Si el bloque es más grande, dividirlo
    else {
        unsigned int tamaño_restante = bloque_libre->tamaño - tamaño;
        
        // Modificar el bloque actual para que sea ocupado
        bloque_libre->tamaño = tamaño;
        bloque_libre->proceso_id = proceso_id;
        bloque_libre->ocupado = 1;
        
        // Crear nuevo bloque libre con el espacio restante
        insertar_bloque(inicio + tamaño, tamaño_restante, 0, 0);
    }
    
    // Actualizar estado del proceso
    proceso->proc.estado = ready;
    
    // Actualizar memoria física
    actualizar_memoria_fisica();
    
    printf("Proceso P%d asignado en memoria (inicio: %d, tamaño: %d).\n", 
           proceso_id, inicio, tamaño);
    return 1;
}

int free_proceso(unsigned int proceso_id, nodo_p_t** head) { //bien/medio
    // Buscar el proceso en la lista
    nodo_p_t* proceso = buscar_proceso_por_id(*head, proceso_id);
    if(proceso == NULL) {
        printf("Error: Proceso P%d no encontrado.\n", proceso_id);
        return 0;
    }
    
    // Buscar el bloque de memoria del proceso
    bloque_memoria_t* bloque_actual = memoria_head;
    while(bloque_actual != NULL) {
        if(bloque_actual->ocupado && bloque_actual->proceso_id == proceso_id) {
            // Liberar el bloque
            bloque_actual->proceso_id = 0;
            bloque_actual->ocupado = 0;
            
            // Actualizar estado del proceso
            if(proceso->proc.estado == ready) {
                proceso->proc.estado = p_new;
            }
            
            // Fusionar bloques libres adyacentes
            //fusionar_bloques_libres(); //quitar creo?
            
            // Actualizar memoria física
            actualizar_memoria_fisica();
            
            printf("Proceso P%d liberado de memoria.\n", proceso_id);
            return 1;
        }
        bloque_actual = bloque_actual->sig;
    }
    
    printf("Error: Proceso P%d no está en memoria.\n", proceso_id);
    return 0;
}

bloque_memoria_t* encontrar_bloque_libre(unsigned int tamaño, estrategia_t estrategia) {//bien
    bloque_memoria_t* mejor_bloque = NULL;
    bloque_memoria_t* actual = memoria_head;
    
    switch(estrategia) {
        case FIRST_FIT:
            while(actual != NULL) {
                if(!actual->ocupado && actual->tamaño >= tamaño) {
                    return actual;
                }
                actual = actual->sig;
            }
            break;
            
        case BEST_FIT:
            while(actual != NULL) {
                if(!actual->ocupado && actual->tamaño >= tamaño) {
                    if(mejor_bloque == NULL || actual->tamaño < mejor_bloque->tamaño) {
                        mejor_bloque = actual;
                    }
                }
                actual = actual->sig;
            }
            break;
            
        case WORST_FIT:
            while(actual != NULL) {
                if(!actual->ocupado && actual->tamaño >= tamaño) {
                    if(mejor_bloque == NULL || actual->tamaño > mejor_bloque->tamaño) {
                        mejor_bloque = actual;
                    }
                }
                actual = actual->sig;
            }
            break;
    }
    
    return mejor_bloque;
}

void insertar_bloque(unsigned int inicio, unsigned int tamaño, unsigned int proceso_id, int ocupado) {//bien
    bloque_memoria_t* nuevo_bloque = (bloque_memoria_t*)malloc(sizeof(bloque_memoria_t));
    nuevo_bloque->inicio = inicio;
    nuevo_bloque->tamaño = tamaño;
    nuevo_bloque->proceso_id = proceso_id;
    nuevo_bloque->ocupado = ocupado;
    nuevo_bloque->sig = NULL;
    
    // Insertar ordenado por posición de inicio
    if(memoria_head == NULL || memoria_head->inicio > inicio) {
        nuevo_bloque->sig = memoria_head;
        memoria_head = nuevo_bloque;
        return;
    }
    
    bloque_memoria_t* actual = memoria_head;
    while(actual->sig != NULL && actual->sig->inicio < inicio) {
        actual = actual->sig;
    }
    
    nuevo_bloque->sig = actual->sig; //El que sigue del nuevo bloque se convierte en NULLO
    actual->sig = nuevo_bloque;
}

void compactar_bloques_libres() { //bien
    bloque_memoria_t* actual = memoria_head;
    
    while(actual != NULL && actual->sig != NULL) {
        // Si el bloque actual y el siguiente están libres y son contiguos
        if(!actual->ocupado && !actual->sig->ocupado && 
           (actual->inicio + actual->tamaño) == actual->sig->inicio) {
            
            bloque_memoria_t* siguiente = actual->sig;
            actual->tamaño += siguiente->tamaño;
            actual->sig = siguiente->sig;
            free(siguiente);
        } else {
            actual = actual->sig;
        }
    }
}

void mostrar_memoria_status() {//ocupa cambios
    printf("\n========== ESTADO DE LA MEMORIA ==========\n");
    printf("Tamaño total: %d bloques\n", MEMORY_SIZE);
    printf("\nBloques de memoria:\n");
    printf("Inicio\tTamaño\tEstado\t\tProceso\n");
    printf("------\t------\t------\t\t-------\n");
    
    bloque_memoria_t* actual = memoria_head;
    int bloques_libres = 0;
    int bloques_ocupados = 0;
    int memoria_libre = 0;
    int memoria_ocupada = 0;
    
    while(actual != NULL) {
        printf("%d\t%d\t", actual->inicio, actual->tamaño);
        
        if(actual->ocupado) {
            printf("OCUPADO\t\tP%d\n", actual->proceso_id);
            bloques_ocupados++;
            memoria_ocupada += actual->tamaño;
        } else {
            printf("LIBRE\t\t-\n");
            bloques_libres++;
            memoria_libre += actual->tamaño;
        }
        
        actual = actual->sig;
    }
    
    printf("\nResumen:\n");
    printf("Bloques libres: %d (total: %d bloques)\n", bloques_libres, memoria_libre);
    printf("Bloques ocupados: %d (total: %d bloques)\n", bloques_ocupados, memoria_ocupada);
    printf("Fragmentación: %.2f%%\n", 
           memoria_libre > 0 ? ((float)bloques_libres - 1) / bloques_libres * 100 : 0);
    printf("==========================================\n\n");
}

void desfragmentar_memoria(nodo_p_t** head) {//no se ocupa
    printf("Iniciando desfragmentacion de memoria...\n");
    
    // Crear nueva lista de bloques compactada
    bloque_memoria_t* nueva_lista = NULL;
    bloque_memoria_t* actual = memoria_head;
    unsigned int posicion_actual = 0;
    int procesos_movidos = 0;
    
    // Primero, colocar todos los bloques ocupados al inicio
    while(actual != NULL) {
        if(actual->ocupado) {
            bloque_memoria_t* nuevo_bloque = (bloque_memoria_t*)malloc(sizeof(bloque_memoria_t));
            nuevo_bloque->inicio = posicion_actual;
            nuevo_bloque->tamaño = actual->tamaño;
            nuevo_bloque->proceso_id = actual->proceso_id;
            nuevo_bloque->ocupado = 1;
            nuevo_bloque->sig = nueva_lista;
            nueva_lista = nuevo_bloque;
            
            posicion_actual += actual->tamaño;
            procesos_movidos++;
        }
        actual = actual->sig;
    }
    
    // Crear un bloque libre con todo el espacio restante
    if(posicion_actual < MEMORY_SIZE) {
        bloque_memoria_t* bloque_libre = (bloque_memoria_t*)malloc(sizeof(bloque_memoria_t));
        bloque_libre->inicio = posicion_actual;
        bloque_libre->tamaño = MEMORY_SIZE - posicion_actual;
        bloque_libre->proceso_id = 0;
        bloque_libre->ocupado = 0;
        bloque_libre->sig = nueva_lista;
        nueva_lista = bloque_libre;
    }
    
    // Liberar la lista anterior
    while(memoria_head != NULL) {
        bloque_memoria_t* temp = memoria_head;
        memoria_head = memoria_head->sig;
        free(temp);
    }
    
    // Asignar la nueva lista ordenada
    memoria_head = NULL;
    while(nueva_lista != NULL) {
        bloque_memoria_t* temp = nueva_lista;
        nueva_lista = nueva_lista->sig;
        
        // Insertar ordenado por inicio
        if(memoria_head == NULL || memoria_head->inicio > temp->inicio) {
            temp->sig = memoria_head;//NULO
            memoria_head = temp;
        } else {
            bloque_memoria_t* aux = memoria_head;
            while(aux->sig != NULL && aux->sig->inicio < temp->inicio) {
                aux = aux->sig;
            }
            temp->sig = aux->sig;
            aux->sig = temp;
        }
    }
    
    // Actualizar memoria física
    actualizar_memoria_fisica();
    
    printf("desfragmentacion terminada. %d procesos reubicados.\n", procesos_movidos);
}

void actualizar_memoria_fisica() {//bien supongo
    // Limpiar memoria
    for(int i = 0; i < MEMORY_SIZE; i++) {
        memoria[i] = BLOCK_FREE;
    }
    
    // Marcar bloques ocupados
    bloque_memoria_t* actual = memoria_head;
    while(actual != NULL) {
        if(actual->ocupado) {
            for(unsigned int i = actual->inicio; i < actual->inicio + actual->tamaño; i++) {
                if(i < MEMORY_SIZE) {
                    memoria[i] = actual->proceso_id;
                }
            }
        }
        actual = actual->sig;
    }
}

nodo_p_t* buscar_proceso_por_id(nodo_p_t* head, unsigned int id) {//bien
    nodo_p_t* actual = head;
    while(actual != NULL) {
        if(actual->proc.id == id) {
            return actual;
        }
        actual = actual->sig;
    }
    return NULL;
}

// Función para convertir string de estrategia a enum
estrategia_t string_to_estrategia(char* str) {//ok
    if(strcmp(str, "first") == 0 || strcmp(str, "firstfit") == 0) {
        return FIRST_FIT;
    } else if(strcmp(str, "best") == 0 || strcmp(str, "bestfit") == 0) {
        return BEST_FIT;
    } else if(strcmp(str, "worst") == 0 || strcmp(str, "worstfit") == 0) {
        return WORST_FIT;
    }
    return FIRST_FIT; // Por defecto
}

// Función para liberar toda la memoria al finalizar el programa
void liberar_memoria_manager() {//bien
    while(memoria_head != NULL) {
        bloque_memoria_t* temp = memoria_head;
        memoria_head = memoria_head->sig;
        free(temp);
    }
}

#endif