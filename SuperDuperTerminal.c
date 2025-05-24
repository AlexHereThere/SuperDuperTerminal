#include<stdio.h>
#include"InComs.h"
#include"Procesadero.h"
int es_com_bash=0;
int es_com_ter=0;



int main()
{
    nodo_p_t* head = NULL;
    char* comandos;
    while(1)
    {
        printf("SuperDuperTerminal> ");
	    comandos = leer_cadena();

        if (strcmp(comandos, "exit") == 0) break;//salir
        Interpret_command(comandos);
        if(es_com_bash)continue;//si fue comando de bash, brincar lo demas.
        manejar_procesos(comandos,&head);
    }
    printf("Adios...\n");
	return 0;
}