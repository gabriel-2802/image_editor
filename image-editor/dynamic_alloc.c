//copyright Carauleanu Valentin Gabriel 311CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aux_functions.h"
#define NMAX 100
#define SIZE 4

double **alloc_matrix(int rws, int cls)
{
	double **mx = malloc(rws * sizeof(double *));
	if (!mx)
		return NULL;

	for (int i = 0; i < rws; ++i) {
		mx[i] = malloc(cls * sizeof(double));
		if (!mx[i]) {
			for (int j = 0; j < i; ++j)
				free(mx[i]);
			free(mx);
			return NULL;
		}
	}
	return mx;
}

void free_matrix(double **mx, int rws)
{
	//eliberam initial fiecare linie
	for (int i = 0; i < rws; ++i)
		free(mx[i]);
	free(mx);
}

void free_image(image_t *image)
{
	if (image->type[1] == '2' || image->type[1] == '5')
		free_matrix(image->grey, image->rws);

	if (image->type[1] == '3' || image->type[1] == '6') {
		free_matrix(image->red, image->rws);
		free_matrix(image->green, image->rws);
		free_matrix(image->blue, image->rws);
	}
	free(image);
}

void check(void *p)
{
	//daca o alocare dinamica esueaza, inchidem programul
	if (!p)
		exit(-1);
}

void initialise_img(image_t **img)
{
	/*la incarcarea unei noi imagini, eliberam din memorie
	imaginea anterioara*/
	if ((*img)) {
		free_image(*img);
		*img = NULL;
	}
}
