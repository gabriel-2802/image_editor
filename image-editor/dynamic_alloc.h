//copyright Carauleanu Valentin Gabriel 311CA
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aux_functions.h"
#define NMAX 100
#define SIZE 4

double **alloc_matrix(int rws, int cls);

void free_matrix(double **mx, int rws);

void free_image(image_t *image);

void check(void *p);

void initialise_img(image_t **img);
