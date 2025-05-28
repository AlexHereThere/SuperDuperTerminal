#include<stdio.h>
#include"lista_procesos.h"

void realizarRR(nodo_p_t* head,int qt); 

void realizarRR(nodo_p_t* head,int qt)
{
    int bt_act,num_proc_ter=0;
    int num_proc = contar_procesos(head);
    int i=0;
    nodo_p_t* nodo_actual = NULL;
    nodo_p_t* nodo_head = head;
    nodo_p_t* aux = head; //para no perder la cabeza original
    unsigned int time=0;
    int procesos_ejecutables = 0;
    printf("\n========== EJECUTANDO RR con QuantumTime %d ==========\n",qt);

    nodo_actual = nodo_head;
    while(nodo_actual!=NULL)
    {
        if(nodo_actual->proc.estado == ready)
        {
            procesos_ejecutables++;
        }
        nodo_actual=nodo_actual->sig;
    }

    if(procesos_ejecutables == 0) {
        printf("No hay procesos en estado READY para ejecutar.\n");
        printf("Use 'alloc <id> <estrategia>' para cargar procesos en memoria.\n");
        return;
    } 

    while(num_proc_ter<num_proc)
    {
        nodo_actual = nodo_head;
        while(nodo_actual!=NULL)
        {
            if(nodo_actual->proc.estado==ready) //si sigue sin terminar
            {
                if(i==0) nodo_actual->proc.waitingTime = time; //actualizar waiting time de proceso actual
                else nodo_actual->proc.waitingTime = time-(qt*i);

                printf("Entra P%d con BT-%d en tiempo %u.\n",
                    nodo_actual->proc.id,
                    nodo_actual->proc.currentBT,
                    time);
                bt_act = nodo_actual->proc.currentBT;

                if(bt_act<=qt)//si burst time es menor que quantum time
                {//termina el proceso (sacar de lista)
                    time+=bt_act;
                    nodo_actual->proc.estado = running;
                    printf("Sale P%d en tiempo %u. (Termino)\n",
                    nodo_actual->proc.id,
                    time);
                    free_proceso(nodo_actual->proc.id, &aux);
                    num_proc_ter++;
                }
                else
                {
                    time+=qt;
                    nodo_actual->proc.currentBT-=qt;
                    printf("Sale P%d en tiempo %u Interrumpido.\n",
                        nodo_actual->proc.id,
                        time);
                }
            }
            nodo_actual=nodo_actual->sig;
        }
        i++;
    }

    printf("\nRR completado. %d procesos ejecutados.\n", procesos_ejecutables);
    aux = ordenar_procesos_ID(aux);
    nodo_p_t* aux2 = aux;
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

