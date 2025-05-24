#include<stdio.h>
#include"lista_procesos.h"

void realizarFCFS(nodo_p_t* head);

void realizarFCFS(nodo_p_t* head)
{
    nodo_p_t* nodo_actual = head;
    nodo_p_t* aux = head; //para no perder la cabeza original
    nodo_p_t* aux2 = head;
    unsigned int time=0;
    while(nodo_actual!=NULL) //hasta que se acaben los procesos
    {
        nodo_actual->proc.waitingTime = time; //actualizar waiting time de proceso actual
        printf("Entra P%d con BT-%d en tiempo %u.\n",
            nodo_actual->proc.id,
            nodo_actual->proc.burstTime,
            time);
        time+=nodo_actual->proc.burstTime;
        nodo_actual->proc.estado=terminated;
        printf("Sale P%d en tiempo %u.\n",
            nodo_actual->proc.id,
            time);
        nodo_actual=nodo_actual->sig;
    }
    printf("=Enter Para Continuar=");
    getchar();
    calcularDatos(aux);
    printf("=Enter Para Continuar=");
    getchar();
    show_all(aux2);
    printf("=Enter Para Continuar=");
    getchar();
    system("clear");
}

