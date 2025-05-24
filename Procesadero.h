#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"infinicadenas.h"
#include"dividecadenas.h"
#include"lista_procesos.h"
#include"FCFS.h"
#include"SJF.h"
#include"RR.h"
//declaracion
void manejar_procesos(char* comando,nodo_p_t** head);
void crear_proceso_manual(nodo_p_t** head,char** c_partes);
void crear_procesos_csv(nodo_p_t** head,char** c_partes);

void manejar_procesos(char* comando,nodo_p_t** head)
{
    int num_com;
    char* str_aux;
    char** c_partes;
    nodo_p_t* aux = NULL;	

    c_partes = tokensToArray(comando," ");

    if(strcmp(c_partes[0],"creaproceso")==0) crear_proceso_manual(head,c_partes); //crear un proceso manualmente
    if(strcmp(c_partes[0],"creaprocesos")==0) crear_procesos_csv(head,c_partes); //crear procesos por csv
    if(strcmp(c_partes[0],"listaprocesos")==0) show_all(*head);
    if(strcmp(c_partes[0],"FCFS")==0)
    {
        if((*head)==NULL)
        {
            printf("Error: no se puede aplicar algoritmo FCFS. (lista de procesos vacio)\n");
            return;
        }
        (*head) = ordenar_procesos_ID(*head);//FCFS solamente ocupa ordenarlos por ID.
        aux = (*head); //resguardar
        realizarFCFS(*head);
        elimina_procesos(aux); //liberar memoria de lista
        *head = NULL;
    } 
    if(strcmp(c_partes[0],"SJF")==0)
    {
        if((*head)==NULL)
        {
            printf("Error: no se puede aplicar algoritmo SJF. (lista de procesos vacio)\n");
            return;
        }
        (*head) = ordenar_procesos_SJF(*head);//SJF ordena de una manera particular.
        aux = (*head);
        realizarSJF(*head);
        elimina_procesos(aux);
        (*head) = NULL;
    }
    if(strcmp(c_partes[0],"RR")==0)
    {
        if((*head)==NULL)
        {
            printf("Error: no se puede aplicar algoritmo RR. (lista de procesos vacio)\n");
            return;
        }
        num_com = arrayLength(c_partes);
        if(num_com==2)//hay 2 parametros en el comando
        {
            unsigned int qt = strtol(c_partes[1],&str_aux,10); //se lee un quantum time
            if(qt<=0) //se asigno un valor invalido como qt.
            {
                printf("Error: quantum time invalido.\n");
                return;
            }
                
            (*head) = ordenar_procesos_ID(*head);
            aux = (*head);
            realizarRR(*head,qt);
            (*head) = NULL;
        }
        else
        {
            printf("Error: falto quantum time o se agrego un parametro de mas.\n");
        }
    }
    freeArray(c_partes); //liberar memoria de instruccion
}

void crear_proceso_manual(nodo_p_t** head,char** c_partes)
{
    char* aux;
    int num_partes = arrayLength(c_partes);
    if(num_partes<2)
    {
        printf("Error: no se ingreso el burst time del proceso, ni el numero de bloques que ocupa.\n");
        return;
    }
    if(num_partes<3)
    {
        printf("Error: no se ingreso el numero de bloques que ocupa.\n");
        return;
    }
    unsigned int bt_value = strtol(c_partes[1],&aux,10);
    if(c_partes[1]==aux || *aux != '\0')
    {
        printf("Error: hubo un error al leer su burst time.\n");
        return;
    }
    unsigned int bz_value = strtol(c_partes[2],&aux,10);
    if(c_partes[2]==aux || *aux != '\0')
    {
        printf("Error: hubo un error al leer su block size.\n");
        return;
    }
    proceso_t new_proceso;
    new_proceso.id = buscar_id_disponible(*head);
    new_proceso.burstTime = bt_value;
    new_proceso.currentBT = bt_value;
    new_proceso.blockSize = bz_value;
    new_proceso.estado = p_new;
    new_proceso.waitingTime = 0;
    in(new_proceso,head);
}

void crear_procesos_csv(nodo_p_t** head,char** c_partes)
{
    char* aux;
    char line[256];
    int num_partes = arrayLength(c_partes);
    if(num_partes<2)
    {
        printf("Error: no se ingreso el csv con los procesos.\n");
        return;
    }
    FILE* file = fopen(c_partes[1],"r");
    if(file == NULL)
    {
        printf("Error: archivo no existe o hubo problema.\n");
        return;
    }
    while(fgets(line,sizeof(line),file))
	{
        char** aux_partes = tokensToArray(line,",");
        if(arrayLength(aux_partes)<2)
        {
            printf("*Error*: Proceso en lista incompleta.\n");
            freeArray(aux_partes); //liberar memoria de instruccion
            fclose(file);
            return; 
        }
        unsigned int bt_value = strtol(aux_partes[0],&aux,10);
        if(bt_value<=0)
        {
            printf("Error: burst time invalido.\n");
            freeArray(aux_partes); //liberar memoria de instruccion
            fclose(file);
            return; 
        }
        unsigned int bz_value = strtol(aux_partes[1],&aux,10);
        if(bz_value<=0)
        {
            printf("Error: block size invalido.\n");
            freeArray(aux_partes); //liberar memoria de instruccion
            fclose(file);
            return;
        }
		proceso_t new_proceso;
        new_proceso.id = buscar_id_disponible(*head);
        new_proceso.burstTime = bt_value;
        new_proceso.currentBT = bt_value;
        new_proceso.blockSize = bz_value;
        new_proceso.estado = p_new;
        new_proceso.waitingTime = 0;
        in(new_proceso,head);
	}
}