#include "automato.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*char *p_transitions;
char** lines = NULL,  ** transitions_seperated = NULL, ** determin_array = NULL;
int size = 0, n_paragraphs = 0, i, j, first_state_transition = 0, reach_state_transition = 0, event_transition = 0, n = 0, parser_state = 0, contador = 0, flag_equal = 0, flag_determin = 0, flag_determin_true = 0, m = 0, x = 0, flag_non_determin = 0, y = 0, ocorrencies = 0;
*/
void parser(automato* load_automata, char* file_info);

void delete_character(char* str,char c)
{
	char *pr = str, *pw = str;
	while (*pr) {
		*pw = *pr++;
		pw += (*pw != c);
	}
	*pw = '\0';
}



void load_file( automato* load_automata ,char* file_path)
{
	FILE* fp;
	int size = 0;
	char* file_info;
	fp = fopen(file_path, "rb");
	if (fp == NULL)
	{
		perror("Error opening file");
		return;
	}
	//get amount of characters in file
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	//load file to a file_info string
	file_info = (char *)malloc(size + 1);
	fread(file_info, sizeof(char), size, fp);
	file_info[size] = '\0';
	fclose(fp);
	parser(load_automata, file_info);
}

bool searcher(char* file_info, char** line, int* index, char delimitator)
{
	int initial_index;

	initial_index = *index;

	if (file_info[*index] == '\0') {
		return false;
	}

	while (true)
	{
		if (file_info[*index] == delimitator)
		{
			*index += 1;
			break;
		}
		*index += 1;
	}

	*line = (char*) realloc(*line, sizeof(char) * (*index - initial_index));
	strncpy(*line, (file_info+initial_index), (*index - initial_index-1));
	(*line)[*index - initial_index - 1]='\0';

	delete_character(*line, 'T');

	return true;
}

void parser(automato* load_automata, char* file_info)
{
	char* line;
	line = (char*)malloc(1);
	int  parser_state = 0, i = 0, index = 0;
	// a cada \r\n o ciclo volta ao inicio


	while (searcher(file_info, &line, &index, '\n'))
	{
		
		printf("%s\n", line);

	}
}



/*
void divide_strings(void)
{
	//dividir a string dos ficheiro original a cada \r\n
	char *  p = strtok(ptr_one, "\r\n");

	// a cada \r\n o ciclo volta ao inicio
	while (p)
	{
		// a cada iteração é realocada memória para o vetor que irá possuir cada linha
		//a memória é realocada com o realocar da iteração anterior e a nova dimensão
		//a cada iteração o número de parágrafos é aumentado e, consequentemente, a dimensão a alocar
		lines = realloc(lines, sizeof(char*) * ++n_paragraphs);

		//confirma a alocação de memória, caso não seja possível encerra o programa
		if (lines == NULL)
			exit(-1);

		//a casa do vetor linesultante, terá a posição da iteração atual, tendo-se de subtrair ao valor 
		//incrementado de dimensão da linha 41
		lines[n_paragraphs - 1] = p;

		//o argumento NULL é passado como primeiro argumento para a função strtok saber que deve procurar
		//na mesma string pelo segundo argumento da função, neste caso, o mesmo \r\n
		p = strtok(NULL, "\r\n");
	}

	//acrescentar mais um espaço de memória para o \0 (NULL element)
	lines = realloc(lines, sizeof(char*) * (n_paragraphs + 1));
	lines[n_paragraphs] = 0;
}



/*
void search_strings(void)
{
	//separar cada uma das componentes do ficheiro

	for (i = 0; i < (n_paragraphs); ++i)
	{


		//separar os estados do resto do ficheiro
		if (strcmp(lines[i], "STATES") == 0)
		{
			parser_state = 1;
			continue;
		}
		//separar os eventos do resto do ficheiro
		if (strcmp(lines[i], "EVENTS") == 0)

		{
			parser_state = 2;
			continue;
		}
		//separar as transições do resto do ficheiro
		if (strcmp(lines[i], "TRANSITIONS") == 0)

		{
			parser_state = 3;
			continue;
		}
		//separar os estados iniciais do resto do ficheiro
		if (strcmp(lines[i], "INITIAL") == 0)

		{
			parser_state = 4;
			continue;
		}
		//separar os estados marcados do resto do ficheiro
		if (strcmp(lines[i], "MARKED") == 0)

		{
			parser_state = 5;
			continue;
		}

		//alocação de memória dinamicamente para cada uma das partes separadas do ficheiro
		switch (parser_state)
		{
		case 1:
			counter_states++;
			states = realloc(states, sizeof(char*) * counter_states);
			if (states == NULL)
				exit(-1);
			states[counter_states - 1] = lines[i];
			break;

		case 2:
			counter_events++;
			events = realloc(events, sizeof(char*) * counter_events);
			if (events == NULL)
				exit(-1);
			events[counter_events - 1] = lines[i];
			break;

		case 3:
			counter_transitions++;
			transitions = realloc(transitions, sizeof(char*) * counter_transitions);
			if (transitions == NULL)
				exit(-1);
			transitions[counter_transitions - 1] = lines[i];
			break;

		case 4:
			counter_initial++;
			initial = realloc(initial, sizeof(char*) * counter_initial);
			if (initial == NULL)
				exit(-1);
			initial[counter_initial - 1] = lines[i];
			break;

		case 5:
			counter_marked++;
			marked = realloc(marked, sizeof(char*) * counter_marked);
			if (marked == NULL)
				exit(-1);
			marked[counter_marked - 1] = lines[i];
			break;

		default:
			printf("\nUnrecognizable file!!\n\n");
		}
	}
}
void write_states(void)
{
	printf("\nStates:\n");
	for (i = 0; i < counter_states; i++)
	{
		printf("%s\n", states[i]);
	}
}
void write_events(void)
{
	printf("\nEvents: \n");
	for (i = 0; i < counter_events; i++)
	{
		printf("%s\n", events[i]);
	}
}
void write_initial_states(void)
{
	printf("\nInitial states: \n");
	for (i = 0; i < counter_initial; i++)
	{
		printf("%s\n", initial[i]);
	}
}
void write_marked_states(void)
{
	printf("\nMarked States: \n");
	for (i = 0; i < counter_marked; i++)
	{
		printf("%s\n", marked[i]);
	}
}
void separate_transitions(void)
{
	i = 0;
	while (i < counter_transitions)
	{
		p_transitions = strtok(transitions[i], ";");

		int size_transitions = 0;
		// a cada \r\n o ciclo volta ao inicio
		while (p_transitions)
		{
			// a cada iteração é realocada memória para o vetor que irá possuir cada linha
			//a memória é realocada com o realocar da iteração anterior e a nova dimensão
			//a cada iteração o número de parágrafos é aumentado e, consequentemente, a dimensão a alocar
			transitions_seperated = realloc(transitions_seperated, sizeof(char*) * ++size_transitions);

			//confirma a alocação de memória, caso não seja possível encerra o programa
			if (transitions_seperated == NULL)
				exit(-1);

			//a casa do vetor linesultante, terá a posição da iteração atual, tendo-se de subtrair ao valor 
			//incrementado de dimensão da linha 41
			transitions_seperated[size_transitions - 1] = p_transitions;

			//o argumento NULL é passado como primeiro argumento para a função strtok saber que deve procurar
			//na mesma string pelo segundo argumento da função, neste caso, o mesmo \r\n
			p_transitions = strtok(NULL, ";");
		}

		for (j = 0; j < size_transitions; j++)
		{
			contador++;
			transicoes = realloc(transicoes, sizeof(char*) * contador);
			if (transicoes == NULL)
				exit(-1);
			transicoes[contador - 1] = transitions_seperated[j];
		}
		i++;
	}
}
int check_automata()
{
	if (contador % 3 != 0)
	{
		printf("\nThe automata has an invalid number of transitions. The file in question doesn't contain an automata as established. ");
		//verificar se é específico de windows
		system("pause");
		exit(-1);
	}
	else
	{
		printf("\n\nValid number of transitions! Checking if the states and events in the transitions are as the stated before:\n");
		return (0);
	}
		
}
void check_first_states(void)
{
	for (i = 0; i < counter_transitions; i++)
	{
		for (j = 0; j < counter_states; j++)
		{
			if (strcmp(states[j], transicoes[i * 3]) == 0)
				first_state_transition++;
		}
	}
	if (first_state_transition == counter_transitions)
		printf("\nThe first states are confirmed\n");
	else
	{
		write_states();
		system("pause");
		exit(-1);
	}
}
void check_reach_states(void)
{
	n = 2;
	for (i = 0; i < counter_transitions; i++)
	{
		for (j = 0; j < counter_states; j++)
		{
			if (i == 0)
			{
				if (strcmp(states[j], transicoes[2]) == 0)
					reach_state_transition++;
			}
			else
			{
				if (strcmp(states[j], transicoes[n]) == 0)
					reach_state_transition++;
			}
		}
		n = n + 3;
	}
	n = 0;

	if (reach_state_transition == counter_transitions)
		printf("\nThe reach states are confirmed\n");
	else
	{
		printf("\nThe reach states are not as the ones established here:\n");
		write_states();
		system("pause");
		exit(-1);
	}
}
void check_events(void)
{
	n = 1;
	for (i = 0; i < counter_transitions; i++)
	{

		for (j = 0; j < counter_events; j++)
		{
			if (i == 0)
			{
				if (strcmp(events[j], transicoes[1]) == 0)
					event_transition++;

			}
			else
			{
				if (strcmp(events[j], transicoes[n]) == 0)
					event_transition++;
			}
		}
		n = n + 3;
	}
	n = 0;

	if (event_transition == counter_transitions)
		printf("\nThe events are confirmed\n");
	else
	{
		printf("\nThe events not as the ones established here:\n");
		write_events();
		system("pause");
		exit(-1);
	}
}
void write_function_form(char f)
{
	printf("\n f(%s", transicoes[f]);
	f++;
	printf(",%s)=", transicoes[f]);
	f++;
	printf("%s\n", transicoes[f]);
	f++;
}
void write_all_function_form()
{
	printf("\nTransitions on the function form:\n");
	y = 0;
	for (x = 0; x < counter_transitions; x++)
	{
		printf("\n f(%s", transicoes[y]);
		y++;
		printf(",%s)=", transicoes[y]);
		y++;
		printf("%s\n", transicoes[y]);
		y++;
	}

}
void check_determin()
{
	i = 0;
	j = 0;
	for (i = 0; i < counter_transitions; i++)
	{
		for (j = 0; j < counter_transitions; j++)
		{
			n = j;
			m = i;
			if (j != i && j>i)
			{
				if (strcmp(transicoes[m = m * 3], transicoes[n = n * 3]) == 0)
					flag_equal = 1;
				if (flag_equal == 1)
				{
					flag_equal = 0;
					m++;
					n++;
					if (strcmp(transicoes[m], transicoes[n]) == 0)
						flag_determin = 1;
					if (flag_determin == 1)
					{
						flag_determin = 0;
						m++;
						n++;
						if (strcmp(transicoes[m], transicoes[n]) == 0)
						{
							flag_determin_true = 1;
						}
						if (flag_determin_true == 1 && flag_non_determin == 0)
						{
							flag_determin_true = 0;
							printf("\nThe automata is deterministic!\n");
						}
						else
						{
							flag_non_determin = 1;
							ocorrencies++;
							if (ocorrencies == 1)
							{
								printf("\nThe automata is non deterministic, because of the transition(s):\n");
								n = n - 2;
								write_function_form(n);
								m = m - 2;
								write_function_form(m);
							}
							if (ocorrencies > 1)
							{
								n = n - 2;
								write_function_form(n);
								m = m - 2;
								write_function_form(m);
							}
						}
					}
				}
			}
		}
	}
}*/
