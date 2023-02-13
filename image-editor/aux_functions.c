//copyright Carauleanu Valentin Gabriel 311CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aux_functions.h"
#define NMAX 100
#define SIZE 4
#define MAXPIXEL 256

void command(int *cmd, char *string)
{
	/*pentru un meniu mai eye-candy vom transforma comanda primita
	in valori numerice */
	char copy[NMAX];
	//folosim a copie pentru a nu denatura string-ul cu strtok
	strcpy(copy, string);
	char *first_word = strtok(copy, " ");

	if (word_count(string) == 2 && !strcmp(first_word, "LOAD"))
		*cmd = 1;
	else if (word_count(string) == 5 && !strcmp(first_word, "SELECT"))
		*cmd = 2;
	else if (!strcmp(string, "SELECT ALL") || !strcmp(string, "SELECT ALL "))
		*cmd = 3;
	else if (!strcmp(first_word, "HISTOGRAM"))
		*cmd = 4;
	else if (!strcmp(string, "EQUALIZE"))
		*cmd = 5;
	else if (!strcmp(string, "CROP"))
		*cmd = 6;
	else if (!strcmp(first_word, "APPLY"))
		*cmd = 7;
	else if ((word_count(string) == 2 || word_count(string) == 3) &&
			 !strcmp(first_word, "SAVE"))
		*cmd = 8;
	else if (!strcmp(string, "EXIT"))
		*cmd = 9;
	else if (!strcmp(first_word, "ROTATE") && word_count(string) == 2)
		*cmd = 10;
	else
		*cmd = -1;
}

char *read_cmd(void)
{
	//citim comanda primita
	char command[NMAX];
	fgets(command, NMAX - 1, stdin);

	//eliminam \n de la finalul string ului citit
	int len = strlen(command);
	if (command[len - 1] == '\n') {
		command[len - 1] = '\0';
		len--;
	}
	//crestem len pentru a copia si \0
	len++;

	char *cmd = malloc(len * sizeof(char));
	if (!cmd)
		return NULL;

	strcpy(cmd, command);
	return cmd;
}

int word_count(char *string)
{
	char copy[NMAX];
	strcpy(copy, string);
	/*utilzam o cope a stringului initial pentru a nu
	altera string ul primit*/

	int count = 0;
	char *word = strtok(copy, " ");
	while (word) {
		count++;
		word = strtok(NULL, " ");
	}

	return count;
}

void load_error(char *f_name)
{
	printf("Failed to load %s\n", f_name);
}

void comment_case(FILE *in)
{
	/*testam existena unui comment, daca acesta exista, vom citi
	toata linia, ignorand-o, daca nu exista, revenim cu o pozitie
	in urma, ignorand citirea primului caracter*/

	char ch = fgetc(in);
	if (ch == '#')
		while (ch != '\n')
			ch = fgetc(in);
	else
		fseek(in, -sizeof(char), SEEK_CUR);
}

void swap(int *a, int *b)
{
	*a = *a ^ *b;
	*b = *a ^ *b;
	*a = *a ^ *b;
}

void copy_matrix(double **dest, double **source, int rws, int cls)
{
	for (int i = 0; i < rws; ++i)
		for (int j = 0; j < cls; ++j)
			dest[i][j] = source[i][j];
}

double clam(double x, double minim, double maxim)
{
	if (x < minim)
		x = minim;

	if (x > maxim)
		x = maxim;

	return x;
}

void submatrix(double **a, double **sub, int rw, int cl, int istrt, int jstrt)
{
	/*vom completa toate elementele din matricea sub cu elemente
	din matricea a pornind de la linia istrt si coloana jstrt*/

	for (int i1 = 0, i2 = istrt; i1 < rw; ++i1, ++i2)
		for (int j1 = 0, j2 = jstrt; j1 < cl; ++j1, ++j2)
			sub[i1][j1] = a[i2][j2];
}

void join_mx(double **a, double **submx, int r, int c, int istrt, int jstrt)
{
	/*vom completa in matricea greater cu toate elementele matricii a,
	pornind de la linia istart si coloana jstart*/

	for (int i1 = 0, i2 = istrt; i1 < r; ++i1, ++i2)
		for (int j1 = 0, j2 = jstrt; j1 < c; ++j1, ++j2)
			a[i2][j2] = submx[i1][j1];
}

int test_selection(int *coord, image_t *img)
{
	//functia va returna 1 daca intreaga imagine este selectata
	if (coord[0] == 0 && coord[1] == img->cls && coord[2] == 0 &&
		coord[3] == img->rws)
		return 1;

	return 0;
}
