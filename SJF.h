#include<stdio.h>
#include"lista_procesos.h"

void realizarSJF(nodo_p_t* head);
nodo_p_t* ordenar_procesos_SJF(nodo_p_t* head);
int buscar_BT_max(nodo_p_t* head);
proceso_t buscar_proceso_max(nodo_p_t* head,int bt_max); 

void realizarSJF(nodo_p_t* head)
{
    nodo_p_t* nodo_actual = head;
    nodo_p_t* aux = head; //para no perder la cabeza original
    unsigned int time = 0;
    int procesos_ejecutados = 0;

    printf("\n========== EJECUTANDO SJF ==========\n");

    while(nodo_actual!=NULL) //hasta que se acaben los procesos
    {
         // Solo ejecutar procesos que están en estado ready (cargados en memoria)
        if(nodo_actual->proc.estado == ready) 
        {
            nodo_actual->proc.waitingTime = time; //actualizar waiting time de proceso actual
            printf("Entra P%d con BT-%d en tiempo %u.\n",
                nodo_actual->proc.id,
                nodo_actual->proc.burstTime,
                time);
            time+=nodo_actual->proc.burstTime;
            nodo_actual->proc.estado=running;
            printf("Sale P%d en tiempo %u.\n",
                nodo_actual->proc.id,
                time);
            // Liberar automáticamente la memoria del proceso terminado
            free_proceso(nodo_actual->proc.id, &aux);
            procesos_ejecutados++;
        }
        nodo_actual=nodo_actual->sig;
    }

    if(procesos_ejecutados == 0) {
        printf("No hay procesos en estado READY para ejecutar.\n");
        printf("Use 'alloc <id> <estrategia>' para cargar procesos en memoria.\n");
        return;
    } else {
        printf("\nSJF completado. %d procesos ejecutados.\n", procesos_ejecutados);
    }

    aux = ordenar_procesos_ID(aux);
    nodo_p_t* aux2 = aux;
    printf("=====================================\n");
    printf("=Enter Para Continuar=");
    getchar();
    calcularDatos(aux);
    printf("Resultados =Enter Para Continuar=");
    getchar();
    show_all(aux2);
    printf("Estado de Procesos =Enter Para Continuar=");
    getchar();
    system("clear");
}

nodo_p_t* ordenar_procesos_SJF(nodo_p_t* head)
{
    int bt_max;
    nodo_p_t* new_head = NULL;
    nodo_p_t* aux_g = head; //para guardar la lista
    nodo_p_t* aux_r = NULL; //para recorrer la lista
    proceso_t aux_proc;
    while(aux_g!=NULL) //hasta que quede vacia la lista
    {
       
        aux_r = aux_g;
        bt_max = buscar_BT_max(aux_r);//buscar mayor BT
        aux_r = aux_g;
        aux_proc = buscar_proceso_max(aux_r,bt_max);//buscar proceso con mayor BT
        rm(aux_proc.id,&aux_g);
        in(aux_proc,&new_head);
    }
    return new_head;
}

int buscar_BT_max(nodo_p_t* head)
{
    int bt_max=0,bt_act;
    nodo_p_t* aux_r = head; //recore desde nueva cabezera.
    while(aux_r!=NULL)
    {
        bt_act = aux_r->proc.burstTime;
        if(bt_act>bt_max)bt_max=bt_act;//si existe otro proceso con el mismo BT, tomara solamente el primero
        aux_r=aux_r->sig;//despues toma el que sigue. (segun su id)
    }
    return bt_max;
}

proceso_t buscar_proceso_max(nodo_p_t* head,int bt_max)
{
    int bt_act;
    nodo_p_t* aux_r = head;
    proceso_t aux_proc; //para mover proceso
    while(aux_r!=NULL)
    {
        bt_act = aux_r->proc.burstTime;
        if(bt_act==bt_max)
        {
            aux_proc = aux_r->proc;   
            break;
        } 
        aux_r=aux_r->sig;
    }
    return aux_proc;
}
