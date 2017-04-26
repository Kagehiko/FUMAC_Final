#pragma once

#include <stdio.h>

typedef struct int_vector
{
	int size;
	int* values;

}int_vector;

typedef struct string_vector
{
	char** string;
	int size;

} string_vector;

typedef struct automato
{
	string_vector states;
	
	string_vector events;
	
	int initial;
	
	int_vector marked;

	int_vector** transitions;

} automato;



void load_file(automato* load_automata, char* file_path);
