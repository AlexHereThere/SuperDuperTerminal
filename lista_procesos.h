#ifndef LISTA_PROCESOS_H
#define LISTA_PROCESOS_H

#include <stdio.h>
#include <stdlib.h>

typedef enum estado{
	p_new,
    ready,
    running,
    terminated
}estado_t;

typedef struct proceso
{
	unsigned int id;
	estado_t estado;
    unsigned int burstTime;
	unsigned int currentBT;
    unsigned int waitingTime;
    unsigned int turnaroundTime;  // Añadido para cálculos más precisos
	unsigned int blockSize;
    unsigned int arrivalTime;     // Añadido para manejar tiempos de llegada
}proceso_t;

typedef struct nodo
{
	proceso_t proc;
	struct nodo* sig;
}nodo_p_t;

void in(proceso_t new_proceso, nodo_p_t** head)
{
	nodo_p_t* aux=NULL;
	if((*head)==NULL)
	{
		(*head)=(nodo_p_t*)malloc(sizeof(nodo_p_t));
		(*head)->proc=new_proceso;//va a ser el primero en la lista
		(*head)->sig=NULL;
	}
	else
	{
		aux=(nodo_p_t*)malloc(sizeof(nodo_p_t));
		aux->proc=new_proceso;//se convierte en el primero en la lista
		aux->sig=(*head);//y apunta al que antes lo era.
		(*head)=aux;
	}
}

void rm(unsigned int id, nodo_p_t** head)
{
	nodo_p_t* aux=NULL;
	if((*head)==NULL) //esta vacia la lista
	{
		printf("ERROR: lista de procesos vacio.\n");
		return;
	}
	else if((*head)->proc.id==id) //el que se desea eliminar es el head
	{
		aux=(*head)->sig;
		free((*head));
		(*head)=aux;	
	}
	else
	{
		if((*head)->sig==NULL)//solamente existe head, y no es lo que se busca eliminar
		{
			printf("ERROR: no existe proceso con ID %d.\n", id);
			return;
		}		
		nodo_p_t* new_next=NULL;
		aux=(*head);
		while(aux->sig != NULL && aux->sig->proc.id!=id)//buscar el proceso con el id
		{
			aux=aux->sig;
		}
		if(aux->sig == NULL) {
			printf("ERROR: no existe proceso con ID %d.\n", id);
			return;
		}
		new_next=aux->sig->sig;
		free(aux->sig);	
		aux->sig=new_next;
	}
}

void show_proc(proceso_t proc)//mostrar un proceso
{
	printf("PID: %d\n",proc.id);
	printf("Burst Time: %d\n",proc.burstTime);
	printf("Block Size: %d\n",proc.blockSize);
	
	switch(proc.estado)
    {
		case p_new:
			printf("Estado: New\n");
			break;
        case ready:
            printf("Estado: Ready (En Memoria)\n");
            break;
        case running:
            printf("Estado: Running\n");
            break;
        case terminated:
            printf("Estado: Terminated\n");
            break;
    }
	printf("Waiting Time: %d\n",proc.waitingTime); 
	printf("Turnaround Time: %d\n",proc.turnaroundTime);
}

void show_all(nodo_p_t* head)//mostrar todos los procesos en la lista
{
	nodo_p_t* aux=NULL;
	if(head==NULL) //esta vacia la lista
	{
		printf("ERROR: lista de procesos vacio.\n");
		return;
	}
	else
	{	
		printf("----------Los Procesos-----------\n");	
		aux=head;
		while(aux!=NULL)
		{	
			show_proc(aux->proc);
			printf("---------------------------------\n");	
			aux=aux->sig;
		}
	}
}

unsigned int buscar_id_disponible(nodo_p_t* head)
{	
	if(head==NULL) return 1;
	
	// Buscar el ID más alto y sumar 1
	unsigned int max_id = 0;
	nodo_p_t* aux = head;
	while(aux != NULL) {
		if(aux->proc.id > max_id) {
			max_id = aux->proc.id;
		}
		aux = aux->sig;
	}
	return max_id + 1;
}

void elimina_procesos(nodo_p_t* head)
{
    nodo_p_t* actual = head;
    nodo_p_t* siguiente;
    while(actual != NULL)
    {
        siguiente = actual->sig;
        free(actual);
        actual = siguiente;
    }
}

void calcularDatos(nodo_p_t* head)
{
    int waitTimeAcum=0,turnTimeAcum=0;
    float waitTimePromedio,turnTimePromedio;
    int waitTime,turnTime;
    int i=0;
    nodo_p_t* aux = head;
    printf("----------Los Resultados---------\n");
    while(aux!=NULL)
    {
        waitTime = aux->proc.waitingTime;
        // Calcular turnaround time correctamente
        turnTime = aux->proc.waitingTime + aux->proc.burstTime;
        aux->proc.turnaroundTime = turnTime; // Actualizar el campo
        
        printf("Wait Time de P%d: %d\n",aux->proc.id,waitTime);
        printf("Turnaround Time de P%d: %d\n",aux->proc.id,turnTime);
        waitTimeAcum+=waitTime;
        turnTimeAcum+=turnTime;
        aux=aux->sig;
        i++;
    }
    
    if(i > 0) {
        waitTimePromedio = (float)waitTimeAcum/i;
        turnTimePromedio = (float)turnTimeAcum/i;
        printf("Waiting Time Promedio: %.2f\n",waitTimePromedio);
        printf("Turnaround Time Promedio: %.2f\n",turnTimePromedio);
    }
    printf("---------------------------------\n");	
}

int contar_procesos(nodo_p_t* head)
{
	int i=0;
	nodo_p_t* aux = head;
	while(aux!=NULL)
	{
		i++;
		aux=aux->sig;
	}
	return i;
}

nodo_p_t* ordenar_procesos_ID(nodo_p_t* head)
{
    printf("Ordenando procesos por ID...\n");

    if (head == NULL) return NULL;

    nodo_p_t* new_head = NULL;
    nodo_p_t* actual;
    nodo_p_t* menor;
    nodo_p_t* anterior_menor;

    while (head != NULL) {
        actual = head;
        menor = head;
        anterior_menor = NULL;

        nodo_p_t* anterior = NULL;
        while (actual != NULL) {
            if (actual->proc.id < menor->proc.id) {
                menor = actual;
                anterior_menor = anterior;
            }
            anterior = actual;
            actual = actual->sig;
        }

        // Quitar menor de head
        if (menor == head) {
            head = head->sig;
        } else {
            anterior_menor->sig = menor->sig;
        }

        // Insertar al final de new_head
        menor->sig = NULL;
        if (new_head == NULL) {
            new_head = menor;
        } else {
            nodo_p_t* temp = new_head;
            while (temp->sig != NULL) temp = temp->sig;
            temp->sig = menor;
        }
    }

    return new_head;
}

#endif