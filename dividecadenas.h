#ifndef DIVIDECADENAS_H
#define DIVIDECADENAS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char** tokensToArray(char* tokens, const char* delim)
{
	char* copia = strdup(tokens);
	int count = 0;
	char* tmp = strtok(copia, delim);

	while (tmp != NULL)
	{
		count++;
		tmp = strtok(NULL, delim);
	}
	free(copia);

	char** array = malloc((count + 1) * sizeof(char*));
	int index = 0;
	tmp = strtok(tokens, delim);
	while (tmp != NULL)
	{
		array[index++] = strdup(tmp);
		tmp = strtok(NULL, delim);
	}
	array[index] = NULL;

	return array;
}

int arrayLength(char** array)
{
	int i = 0;
	while (array[i] != NULL)
	{
		i++;
	}
	return i;
}

void freeArray(char** array)
{
	if (!array) return;
	for (int i = 0; array[i] != NULL; i++)
	{
		free(array[i]);
	}
	free(array);
}

#endif

