//copyright Carauleanu Valentin Gabriel 311CA
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "aux_functions.h"
#include "dynamic_alloc.h"
#define NMAX 100
#define SIZE 4
#define MAXPIXEL 256
#define K 3

image_t *load_txt(char *type, int rws, int cls, int max, FILE * in);

image_t *load_bin(char *type, int rws, int cls, int max, FILE *in, char *name);

void load_cmd(char *cmd_string, image_t **image);

void select_all(int *coord, image_t *img);

void select_all_cmd(image_t *img, int *coord);

void select_cmd(char *cmd_string, image_t *img, int coord[SIZE]);

void print_hist(image_t *img, int stars, int bins);

void histogram_cmd(image_t *img, char *cmd_string);

double eq_sum(int *freq, double pixel);

double eq_edit(int *freq, double area, double pixel);

void equalize_cmd(image_t *img);

image_t *crop(image_t *img, int *coord);

void crop_cmd(image_t **img, int *coord);

void save_txt(image_t *img, char *f_name);

void save_bin(image_t *img, char *f_name);

void save_cmd(image_t *img, char *cmd_string);

double apply_pixel(double **aux, double ker[K][K]);

void apy(double **a, double ker[K][K], int *coord, double **src,  int r, int c);

image_t *apyllied(image_t *img, int *coord, double ker[K][K]);

void apply_cmd(image_t **img, int *coord, char *cmd_string);

double **rotate_left(double **mx, int rws, int cls);

double **rotate_right(double **mx, int rws, int cls);

void rotate_mx(double ***source, int rws, int cls, rotate direction);

void rotate_selection(image_t *img, int rotations, int *coord, rotate direct);

image_t *rotate_image(image_t *img, int rotations, rotate direction);

void rotate_cmd(image_t **img, int *coord, char *cmd_string, int *rotated);
