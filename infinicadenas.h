#ifndef INFINICADENAS_H
#define INFINICADENAS_H

#include <stdio.h>
#include <stdlib.h>

char* leer_cadena()
{
	int c; //representa char leido
	int capacidad = 10; //capacidad inicial
	int longitud = 0; //longitud incial de lo que se lee
	char* cadena =(char*)malloc(capacidad * sizeof(char));
	if(!cadena)
	{
		perror("malloc");//hubo error
		return NULL;
	}

	while((c = getchar()) != '\n' && c != EOF)
	{
		if(longitud+1>=capacidad)//exedio la capacidad (hacer espacio para '\0'
		{
			capacidad += 10;//aumentar capacidad con realloc
			char* nueva_cadena = (char*)realloc(cadena, capacidad * sizeof(char));
			if(!nueva_cadena)
			{
				free(cadena);
				perror("realloc");//hubo error
				return NULL;
			}
			cadena = nueva_cadena;//hacer cadena con longitud mas grande
		}
		cadena[longitud++] = (char)c;
	}
	cadena[longitud] = '\0'; //terminar la cadena
	return cadena;
}

#endif