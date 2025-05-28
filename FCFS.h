#include<stdio.h>
#include"lista_procesos.h"
#include"memory_manager.h"

void realizarFCFS(nodo_p_t* head);

void realizarFCFS(nodo_p_t* head)
{
    nodo_p_t* nodo_actual = head;
    nodo_p_t* aux = head; //para no perder la cabeza original
    unsigned int time = 0;
    int procesos_ejecutados = 0;
    
    printf("\n========== EJECUTANDO FCFS ==========\n");
    
    while(nodo_actual != NULL) //hasta que se acaben los procesos
    {
        // Solo ejecutar procesos que están en estado ready (cargados en memoria)
        if(nodo_actual->proc.estado == ready) {
            // El waiting time es el tiempo actual menos el tiempo que llegó (asumimos arrival time = 0)
            nodo_actual->proc.waitingTime = time;
            
            printf("Entra P%d con BT-%d en tiempo %u.\n",
                nodo_actual->proc.id,
                nodo_actual->proc.burstTime,
                time);
            
            time += nodo_actual->proc.burstTime;
            nodo_actual->proc.estado = running;
            
            printf("Sale P%d en tiempo %u.\n",
                nodo_actual->proc.id,
                time);
            
            // Liberar automáticamente la memoria del proceso terminado
            free_proceso(nodo_actual->proc.id, &aux);
            procesos_ejecutados++;
        }    
        nodo_actual = nodo_actual->sig;
    }
    
    if(procesos_ejecutados == 0) {
        printf("No hay procesos en estado READY para ejecutar.\n");
        printf("Use 'alloc <id> <estrategia>' para cargar procesos en memoria.\n");
        return;
    } else {
        printf("\nFCFS completado. %d procesos ejecutados.\n", procesos_ejecutados);
    }
    
    printf("=====================================\n");
    printf("=Enter Para Continuar=");
    getchar();
    calcularDatos(aux);
    printf("Resultados =Enter Para Continuar=");
    getchar();
    show_all(aux);
    printf("Estado de Procesos =Enter Para Continuar=");
    getchar();
    system("clear");
}