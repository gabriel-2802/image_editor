//copyright Carauleanu Valentin Gabriel 311CA
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aux_functions.h"
#define NMAX 100
#define SIZE 4
#define MAXPIXEL 256

typedef struct __attribute__((__packed__)) {
	char type[SIZE];
	int cls, rws, max;
	double **grey;
	double **red, **green, **blue;
	/*in cazul unei imagine greyscale, vom memora matricea imaginii
	in **grey, iar in cazul unei imagini colore vom folosi **red,
	**green, **blue*/
} image_t;

//vom folosi rotate pentru pointeri la functiile rotate_left si rotate_right
typedef double **(*rotate)(double **, int, int);

char *read_cmd(void);

int word_count(char *string);

void command(int *cmd, char *string);

void load_error(char *f_name);

void comment_case(FILE *in);

void swap(int *a, int *b);

void copy_matrix(double **dest, double **source, int rws, int cls);

double clam(double x, double minim, double maxim);

void submatrix(double **a, double **sub, int rw, int cl, int istrt, int jstrt);

void join_mx(double **a, double **submx, int r, int c, int istrt, int jstrt);

int test_selection(int *coord, image_t *img);
