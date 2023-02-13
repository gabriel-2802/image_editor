//copyright Carauleanu Valentin Gabriel 311CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "aux_functions.h"
#include "dynamic_alloc.h"
#define NMAX 100
#define SIZE 4
#define MAXPIXEL 256
#define K 3

image_t *load_txt(char *type, int rws, int cls, int max, FILE *in)
{
	image_t *img = malloc(1 * sizeof(image_t));
	if (!img)
		return NULL;

	//completam filed urile structurii cu elementele citite anterior
	strcpy(img->type, type);
	img->rws = rws;
	img->cls = cls;
	img->max = max;

	if (img->type[1] == '2') {
		//imaginea este greyscale, deci citim in matricea grey
		img->grey = alloc_matrix(rws, cls);
		check(img->grey);

		for (int i = 0; i < rws; ++i)
			for (int j = 0; j < cls; ++j)
				fscanf(in, "%lf", &img->grey[i][j]);
	}

	if (img->type[1] == '3') {
		//imaginea este color, deci citim cele 3 matrice RGB
		img->red = alloc_matrix(rws, cls);
		check(img->red);

		img->green = alloc_matrix(rws, cls);
		check(img->green);

		img->blue = alloc_matrix(rws, cls);
		check(img->blue);

		for (int i = 0; i < rws; ++i)
			for (int j = 0; j < cls; ++j) {
				fscanf(in, "%lf", &img->red[i][j]);
				fscanf(in, "%lf", &img->green[i][j]);
				fscanf(in, "%lf", &img->blue[i][j]);
			}
	}

	fclose(in);
	return img;
}

image_t *load_bin(char *type, int rws, int cls, int max, FILE *in, char *name)
{
	image_t *img = malloc(1 * sizeof(image_t));
	if (!img)
		return NULL;

	//completam field urile structurii cu elementele citite anterior
	strcpy(img->type, type);
	img->rws = rws;
	img->cls = cls;
	img->max = max;

	/*inchidem fisierul inainte de citirea matricelor, si il redeschidem
	la pozitia de la inchidere salvata in position*/
	fpos_t position;
	fgetpos(in, &position);
	fclose(in);
	in = fopen(name, "rb");
	fsetpos(in, &position);

	if (img->type[1] == '5') {
		img->grey = alloc_matrix(rws, cls);
		check(img->grey);

		unsigned char ch;
		for (int i = 0; i < rws; ++i)
			for (int j = 0; j < cls; ++j) {
				fread(&ch, 1, sizeof(unsigned char), in);
				img->grey[i][j] = (double)ch;
			}
	}

	if (type[1] == '6') {
		//imaginea este color, deci citim cele 3 matrice RGB
		img->red = alloc_matrix(rws, cls);
		check(img->red);

		img->green = alloc_matrix(rws, cls);
		check(img->green);

		img->blue = alloc_matrix(rws, cls);
		check(img->blue);

		unsigned char ch;
		for (int i = 0; i < rws; ++i)
			for (int j = 0; j < cls; ++j) {
				fread(&ch, 1, sizeof(unsigned char), in);
				img->red[i][j] = (double)ch;

				fread(&ch, 1, sizeof(unsigned char), in);
				img->green[i][j] = (double)ch;

				fread(&ch, 1, sizeof(unsigned char), in);
				img->blue[i][j] = (double)ch;
			}
	}

	fclose(in);
	return img;
}

void load_cmd(char *cmd_string, image_t **image)
{
	//obtinem numele fisierului
	char *f_name = strtok(cmd_string, " ");
	f_name = strtok(NULL, " ");

	char name[NMAX];
	strcpy(name, f_name);
	FILE *in = NULL;
	in = fopen(name, "rt");
	if (!in) {
		load_error(name);
		return;
	}

	//salvam magic word-ul, inclusiv \n de la final
	char type[SIZE];
	fgets(type, SIZE, in);

	comment_case(in);
	int cls, rws;
	fscanf(in, "%d%d", &cls, &rws);

	//trecem peste \n de la finalul randului, pentru a verifica exitena unui #
	fseek(in, sizeof(char), SEEK_CUR);
	comment_case(in);
	int max;
	fscanf(in, "%d", &max);

	//verificam existena unui ultim comentariu inainte de matricea imaginii
	fseek(in, sizeof(char), SEEK_CUR);
	comment_case(in);

	if (type[1] == '2' || type[1] == '3') {
		//imaginea este text
		*image = load_txt(type, rws, cls, max, in);
	} else if (type[1] == '5' || type[1] == '6') {
		//imaginea este in format binar
		*image = load_bin(type, rws, cls, max, in, name);
	} else {
		//formatul imaginii nu este valid
		load_error(name);
	}

	if (*image)
		printf("Loaded %s\n", name);
	else
		load_error(name);
}

void select_all(int *coord, image_t *img)
{
	//x1 = coord[0], x2 = coord[1], y1 = coord[2], y2 = coord[3]
	for (int i = 0; i < SIZE; ++i)
		coord[i] = 0;
	coord[1] = img->cls;
	coord[3] = img->rws;
}

void select_all_cmd(image_t *img, int *coord)
{
	if (!img) {
		printf("No image loaded\n");
		return;
	}

	select_all(coord, img);
	printf("Selected ALL\n");
}

void select_cmd(char *cmd_string, image_t *img, int coord[SIZE])
{
	/*vom folosi un vector auxiliar pentru a pastra posibilele
	coord din comanda pentru a nu pierde selectia anterioara in
	cazul unor coord invalide*/
	int aux_coord[SIZE];
	for (int i = 0; i < SIZE; ++i)
		aux_coord[i] = -1;

	/*trecem peste cuvantul SELECT si neavand nevoie mai departe
	de cmd_string nu vom folosi o copie*/
	char *word = strtok(cmd_string, " ");
	word = strtok(NULL, " ");
	int i = 0;

	while (word) {
		/*ne asiguram ca token-ul(cuvantul) are doar cifre
		pentru a nu salva comenzi invalide de forma
		SELECT 1X 2 3 4*/
		int len = strlen(word);
		for (int j = 0; j < len; ++j)
			if (isalpha(word[j])) {
				printf("Invalid command\n");
				return;
			}

		/*daca cuvantul este valid(are doar cifre), il
		transformam intr-un numar si il adaugam in aux_coord*/
		aux_coord[i++] = atoi(word);
		word = strtok(NULL, " ");
	}

	//ajungand aici, comanda data esta valida
	if (!img) {
		printf("No image loaded\n");
		return;
	}

	for (int i = 0; i < SIZE; ++i)
		if (aux_coord[i] < 0) {
			printf("Invalid set of coordinates\n");
			return;
		}

	//modificam vectorul a.i. sa contina elementele x1, x2, y1, y2
	swap(&aux_coord[1], &aux_coord[2]);

	if ((aux_coord[0] > img->cls || aux_coord[1] > img->cls) ||
		(aux_coord[2] > img->rws || aux_coord[3] > img->rws) ||
		(aux_coord[0] == aux_coord[1] || aux_coord[2] == aux_coord[3])) {
		printf("Invalid set of coordinates\n");
		return;
	}

	for (int i = 0; i < SIZE; i += 2)
		if (aux_coord[i] > aux_coord[i + 1])
			swap(&aux_coord[i], &aux_coord[i + 1]);

	/*daca am ajuns aici, aux_coord contine coordonate valide de forma:
	x1, x2, y1, y2 cu x1 < x2 si y1 < y2, deci putem sa memoram noile coord
	in vectorul principal*/
	for (int i = 0; i < SIZE; ++i)
		coord[i] = aux_coord[i];

	printf("Selected %d %d %d %d\n", coord[0], coord[2], coord[1], coord[3]);
}

void print_hist(image_t *img, int stars, int bins)
{
	int freq[MAXPIXEL] = {0}, freq_max = -1;
	for (int i = 0; i  < img->rws; ++i)
		for (int j = 0; j < img->cls; ++j)
			freq[(int)img->grey[i][j]]++;

	//intervalul va fi nr de valori / bin
	int interval = MAXPIXEL / bins;

	//aflam frecventa maxima dintr-un bin
	for (int i = 0; i < MAXPIXEL; i += interval) {
		int freq_val = 0;
		for (int j = i; j < (i + interval); ++j)
			freq_val += freq[j];
		if (freq_max < freq_val)
			freq_max = freq_val;
	}

	for (int i = 0; i < MAXPIXEL; i += interval) {
		int freq_val = 0;
		for (int j = i; j < (i + interval); ++j)
			freq_val += freq[j];

		int stars_count = (int)(1.0 * freq_val / freq_max * stars);

		printf("%d\t|\t", stars_count);
		for (int k = 0; k < stars_count; ++k)
			printf("*");
		printf("\n");
	}
}

void histogram_cmd(image_t *img, char *cmd_string)
{
	if (!img) {
		printf("No image loaded\n");
		return;
	}

	if (word_count(cmd_string) != 3) {
		printf("Invalid command\n");
		return;
	}
	char copy[NMAX];
	strcpy(copy, cmd_string);
	char *word = strtok(copy, " ");
	word = strtok(NULL, " ");

	while (word) {
		int len = strlen(word);
		for (int i = 0; i < len; ++i)
			if (isalpha(word[i])) {
				printf("Invalid command\n");
				return;
			}
		word = strtok(NULL, " ");
	}

	//daca am ajuns aici, comanda este valida

	if (img->type[1] == '3' || img->type[1] == '6') {
		printf("Black and white image needed\n");
		return;
	}

	/*aflam numarul de stelulte si binuri: al doilea cuvant va fi nr de
	stars, iar al treilea cuvant numarul de bins*/
	int stars, bins;
	word = strtok(cmd_string, " ");
	word = strtok(NULL, " ");
	stars = atoi(word);
	word = strtok(NULL, " ");
	bins = atoi(word);

	print_hist(img, stars, bins);
}

double eq_sum(int *freq, double pixel)
{
	/*functia calculeaza suma tuturor aparitiilor
	pixelilor mai mici sau egali decat pixel*/
	int s = 0;
	for (int i = 0; i <= (int)pixel; ++i)
		s += freq[i];

	return (double)s;
}

double eq_edit(int *freq, double area, double pixel)
{
	double new_pixel = 255 / area * eq_sum(freq, pixel);
	return round(clam(new_pixel, 0, MAXPIXEL - 1));
}

void equalize_cmd(image_t *img)
{
	if (!img) {
		printf("No image loaded\n");
		return;
	}

	if (img->type[1] == '3' || img->type[1] == '6') {
		printf("Black and white image needed\n");
		return;
	}

	//daca am ajuns aici, imaginea este greyscale
	int freq[MAXPIXEL] = {0};

	for (int i = 0; i < img->rws; ++i)
		for (int j = 0; j < img->cls; ++j)
			freq[(int)img->grey[i][j]]++;

	double area = img->rws * img->cls;

	for (int i = 0; i < img->rws; ++i)
		for (int j = 0; j < img->cls; ++j)
			img->grey[i][j] = eq_edit(freq, area, img->grey[i][j]);

	printf("Equalize done\n");
}

image_t *crop(image_t *img, int *coord)
{
	image_t *crop = malloc(1 * sizeof(image_t));
	check(crop);

	/*vom crea o noua structura imagine, cu dimensiunile
	si matricele noi selectate*/
	strcpy(crop->type, img->type);
	/*noile dimensiuni vor fi: rws = y2 - y1, iar cls =
	x2 - x1*/
	crop->rws = coord[3] - coord[2];
	crop->cls = coord[1] - coord[0];
	crop->max = img->max;

	//pentru a nu depasi 80 de coloane vom folosi rws, cls
	int rws = crop->rws, cls = crop->cls;
	if (img->type[1] == '2' || img->type[1] == '5') {
		crop->grey = alloc_matrix(crop->rws, crop->cls);
		check(crop->grey);
		//noua imagine va contine o submatrice a imaginii initale
		submatrix(img->grey, crop->grey, rws, cls, coord[2], coord[0]);
	} else {
		crop->red = alloc_matrix(crop->rws, crop->cls);
		check(crop->red);
		submatrix(img->red, crop->red, rws, cls, coord[2], coord[0]);

		crop->green = alloc_matrix(crop->rws, crop->cls);
		check(crop->green);
		submatrix(img->green, crop->green, rws, cls, coord[2], coord[0]);

		crop->blue = alloc_matrix(crop->rws, crop->cls);
		check(crop->blue);
		submatrix(img->blue, crop->blue, rws, cls, coord[2], coord[0]);
	}

	return crop;
}

void crop_cmd(image_t **img, int *coord)
{
	if (!(*img)) {
		printf("No image loaded\n");
		return;
	}

	//daca selectia este ALL, terminam comanda
	if (coord[0] == 0 && coord[1] == (*img)->cls && coord[2] == 0 &&
		coord[3] == (*img)->rws) {
		printf("Image cropped\n");
		return;
	}

	image_t *cropped = crop(*img, coord);
	free_image(*img);
	*img = cropped;
	printf("Image cropped\n");
}

void save_txt(image_t *img, char *f_name)
{
	FILE *out = fopen(f_name, "wt");
	if (!out) {
		printf("No image loaded\n");
		return;
	}

	if (img->type[1] == '2' || img->type[1] == '5') {
		//imaginea este greyscale
		fprintf(out, "%s", "P2\n");
		fprintf(out, "%d %d\n%d\n", img->cls, img->rws, img->max);

		for (int i = 0; i < img->rws; ++i) {
			for (int j = 0; j < img->cls; ++j)
				fprintf(out, "%d ", (int)img->grey[i][j]);
			fprintf(out, "\n");
		}
	} else {
		//imaginea este color
		fprintf(out, "%s", "P3\n");
		fprintf(out, "%d %d\n%d\n", img->cls, img->rws, img->max);

		for (int i = 0; i < img->rws; ++i) {
			for (int j = 0; j < img->cls; ++j) {
				fprintf(out, "%d ", (int)img->red[i][j]);
				fprintf(out, "%d ", (int)img->green[i][j]);
				fprintf(out, "%d ", (int)img->blue[i][j]);
			}
			fprintf(out, "\n");
		}
	}

	fclose(out);
}

void save_bin(image_t *img, char *f_name)
{
	FILE *out = fopen(f_name, "wt");
	if (!out) {
		printf("No image loaded\n");
		return;
	}

	/*pentru a resepcta formatul ppm/pbm magic word-ul, nr de linii,
	coloane si val max trebuie sa fie in format txt, iar matricele
	in format binar. Astfel, dupa scrierea datelor, memoram pozitia
	cursorului, inchidem fisierul si il redeschidem ca format "ab"
	pentru a scrie in binar matricele*/

	if (img->type[1] == '2' || img->type[1] == '5') {
		//imaginea este greyscale
		fprintf(out, "%s", "P5\n");
		fprintf(out, "%d %d\n%d\n", img->cls, img->rws, img->max);
	} else {
		//imaginea este color
		fprintf(out, "%s", "P6\n");
		fprintf(out, "%d %d\n%d\n", img->cls, img->rws, img->max);
	}

	fpos_t position;
	fgetpos(out, &position);
	fclose(out);
	out = fopen(f_name, "ab");
	fsetpos(out, &position);

	if (img->type[1] == '2' || img->type[1] == '5') {
		for (int i = 0; i < img->rws; ++i)
			for (int j = 0; j < img->cls; ++j) {
				char ch = (char)img->grey[i][j];
				fwrite(&ch, 1, sizeof(char), out);
			}
	} else {
		for (int i = 0; i < img->rws; ++i)
			for (int j = 0; j < img->cls; ++j) {
				char ch = (char)img->red[i][j];
				fwrite(&ch, 1, sizeof(char), out);

				ch = (char)img->green[i][j];
				fwrite(&ch, 1, sizeof(char), out);

				ch = (char)img->blue[i][j];
				fwrite(&ch, 1, sizeof(char), out);
			}
	}
	fclose(out);
}

void save_cmd(image_t *img, char *cmd_string)
{
	/*izolam numele fisierului prin alegerea
	celui de al doilea cuvant din copia cmd_string*/
	char copy[NMAX];
	strcpy(copy, cmd_string);

	char *word = strtok(copy, " ");
	word = strtok(NULL, " ");
	char f_name[NMAX];
	strcpy(f_name, word);

	//retinem posibilul ultim cuvant in word
	word = strtok(NULL, " ");

	//ne asiguram ca structura comenzii este corecta
	if (word_count(cmd_string) == 3) {
		if (strcmp(word, "ascii")) {
			printf("Invalid command\n");
			return;
		}
	}

	if (!img) {
		printf("No image loaded\n");
		return;
	}

	if (word_count(cmd_string) == 2)
		save_bin(img, f_name);
	else
		save_txt(img, f_name);

	printf("Saved %s\n", f_name);
}

double apply_pixel(double **mx, double ker[K][K])
{
	double pixel = 0;
	/*inmultim elementele celor 2 matrici pentru a obtine
	valoarea noului pixel*/
	for (int i = 0; i < K; ++i)
		for (int j = 0; j < K; ++j)
			pixel += (mx[i][j] * ker[i][j]);

	return round(clam(pixel, 0, MAXPIXEL - 1));
}

void apy(double **a, double ker[K][K], int *coord, double **src,  int r, int c)
{
	double **aux = alloc_matrix(K, K);
	check(aux);

	/*vom aplica kernel-ul tuturor elementelor care nu sunt
	pe margine*/
	for (int i = coord[2]; i < coord[3]; ++i)
		for (int j = coord[0]; j < coord[1]; ++j)
			if (i != 0 && j != 0 && i != (r - 1) && j != (c - 1)) {
				/*aux va contine submatricea de 3 * 3 centrata in
				elementul curent*/
				submatrix(src, aux, K, K, i - 1, j - 1);
				a[i][j] = apply_pixel(aux, ker);
			}
	free_matrix(aux, K);
}

image_t *appllied(image_t *img, int *coord, double ker[K][K])
{
	//vom crea o noua imagine care contine matricele new_img
	image_t *new_img = malloc(1 * sizeof(image_t));
	check(new_img);

	strcpy(new_img->type, img->type);
	new_img->cls = img->cls;
	new_img->rws = img->rws;
	new_img->max = img->max;

	if (img->type[1] == '2' && img->type[1] == '5') {
		//imaginea este greyscale
		new_img->grey = alloc_matrix(new_img->rws, new_img->cls);
		check(new_img->grey);

		copy_matrix(new_img->grey, img->grey, img->rws, img->cls);
		apy(new_img->grey, ker, coord, img->grey, img->rws, img->cls);
	} else {
		//imaginea este color
		new_img->red = alloc_matrix(new_img->rws, new_img->cls);
		check(new_img->red);

		copy_matrix(new_img->red, img->red, img->rws, img->cls);
		apy(new_img->red, ker, coord, img->red, img->rws, img->cls);

		new_img->green = alloc_matrix(new_img->rws, new_img->cls);
		check(new_img->green);

		copy_matrix(new_img->green, img->green, img->rws, img->cls);
		apy(new_img->green, ker, coord, img->green, img->rws, img->cls);

		new_img->blue = alloc_matrix(new_img->rws, new_img->cls);
		check(new_img->blue);

		copy_matrix(new_img->blue, img->blue, img->rws, img->cls);
		apy(new_img->blue, ker, coord, img->blue, img->rws, img->cls);
	}

	return new_img;
}

void apply_cmd(image_t **img, int *coord, char *cmd_string)
{
	if (!(*img)) {
		printf("No image loaded\n");
		return;
	}

	char copy[NMAX];
	strcpy(copy, cmd_string);
	char *parameter = strtok(cmd_string, " ");
	parameter = strtok(NULL, " ");

	if (!parameter) {
		printf("Invalid command\n");
		return;
	}

	if ((*img)->type[1] == '2' || (*img)->type[1] == '5') {
		printf("Easy,  Charlie Chaplin\n");
		return;
	}

	char parameters[] = "EDGESHARPENBLURGAUSSIAN_BLUR\0";
	//testam existenta unui parametru valid
	if (!strstr(parameters, parameter)) {
		printf("APPLY parameter invalid\n");
		return;
	}

	/*datorita marimii mici a matricelor kernel, nu este nevoie de o
	alocare dinamica a acestora*/
	double edge[K][K] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}},
	sharpen[K][K] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}},
	blur[K][K] = {{1, 1, 1}, {1, 1, 1}, {1, 1, 1}},
	gauss[K][K] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};

	for (int i = 0; i < K; ++i)
		for (int j = 0; j < K; ++j)
			blur[i][j] *= (1.0 / 9.0);

	for (int i = 0; i < K; ++i)
		for (int j = 0; j < K; ++j)
			gauss[i][j] *= (1.0 / 16.0);

	image_t *new_img;

	if (!strcmp(parameter, "EDGE"))
		new_img = appllied(*img, coord, edge);
	else if (!strcmp(parameter, "SHARPEN"))
		new_img = appllied(*img, coord, sharpen);
	else if (!strcmp(parameter, "BLUR"))
		new_img = appllied(*img, coord, blur);
	else if (!strcmp(parameter, "GAUSSIAN_BLUR"))
		new_img = appllied(*img, coord, gauss);

	free_image(*img);
	*img = new_img;
	printf("APPLY %s done\n", parameter);
}

double **rotate_left(double **mx, int rws, int cls)
{
	int new_rws = cls, new_cls = rws;
	double **rotate = alloc_matrix(new_rws, new_cls);
	check(rotate);

	for (int i = 0; i < new_rws; ++i)
		for (int j = 0; j < new_cls; ++j)
			rotate[i][j] = mx[j][cls - i - 1];

	return rotate;
}

double **rotate_right(double **mx, int rws, int cls)
{
	int new_rws = cls, new_cls = rws;
	double **rotate = alloc_matrix(new_rws, new_cls);
	check(rotate);

	for (int i = 0; i < new_rws; ++i)
		for (int j = 0; j < new_cls; ++j)
			rotate[i][j] = mx[rws - j - 1][i];

	return rotate;
}

void rotate_mx(double ***source, int rws, int cls, rotate direction)
{
	/*functia roteste matricea in directia dorita, interschimband
	liniile si coloanele*/
	double **aux = direction(*source, rws, cls);
	free_matrix(*source, rws);

	swap(&rws, &cls);
	*source = alloc_matrix(rws, cls);
	copy_matrix(*source, aux, rws, cls);
	free_matrix(aux, rws);
}

void rotate_selection(image_t *img, int rotations, int *coord, rotate direct)
{
	// selectia este patratica, deci este necesara o singura dimensiune
	int dim = coord[1] - coord[0];

	if (img->type[1] == '2' || img->type[1] == '5') {
		/*copiem zona selectata in matricea grey si o rotim
		folosind matricea source*/
		double **source = alloc_matrix(dim, dim);
		check(source);
		submatrix(img->grey, source, dim, dim, coord[2], coord[0]);

		while (rotations) {
			rotate_mx(&source, dim, dim, direct);
			rotations--;
		}

		/*suprascriem matricea rotita peste zona selectata din
		matricea originala*/
		join_mx(img->grey, source, dim, dim, coord[2], coord[0]);
		free_matrix(source, dim);
	} else {
		/*in cazul unei imagini color vom proceda identic, dar cu cele
		3 matrice RGB*/
		double **source_r = alloc_matrix(dim, dim);
		double **source_g = alloc_matrix(dim, dim);
		double **source_b = alloc_matrix(dim, dim);
		check(source_r);
		check(source_g);
		check(source_b);

		submatrix(img->red, source_r, dim, dim, coord[2], coord[0]);
		submatrix(img->green, source_g, dim, dim, coord[2], coord[0]);
		submatrix(img->blue, source_b, dim, dim, coord[2], coord[0]);

		while (rotations) {
			rotate_mx(&source_r, dim, dim, direct);
			rotate_mx(&source_g, dim, dim, direct);
			rotate_mx(&source_b, dim, dim, direct);
			rotations--;
		}

		join_mx(img->red, source_r, dim, dim, coord[2], coord[0]);
		free_matrix(source_r, dim);

		join_mx(img->green, source_g, dim, dim, coord[2], coord[0]);
		free_matrix(source_g, dim);

		join_mx(img->blue, source_b, dim, dim, coord[2], coord[0]);
		free_matrix(source_b, dim);
	}
}

image_t *rotate_image(image_t *img, int rotations, rotate direction)
{
	image_t *new = malloc(1 * sizeof(image_t));
	check(new);

	// cream o noua structura imagine ce contine matricele rotite
	strcpy(new->type, img->type);
	new->max = img->max;
	int rws = img->rws, cls = img->cls;

	if (new->type[1] == '2' || new->type[1] == '5') {
		// imaginea este greyscale
		double **source = alloc_matrix(rws, cls);
		check(source);
		copy_matrix(source, img->grey, rws, cls);

		while (rotations) {
			rotate_mx(&source, rws, cls, direction);
			swap(&rws, &cls);
			rotations--;
		}

		new->rws = rws;
		new->cls = cls;
		new->grey = alloc_matrix(rws, cls);
		copy_matrix(new->grey, source, rws, cls);
		free_matrix(source, rws);
	} else {
		// imaginea este color
		double **source_r = alloc_matrix(rws, cls);
		double **source_g = alloc_matrix(rws, cls);
		double **source_b = alloc_matrix(rws, cls);
		check(source_r);
		check(source_g);
		check(source_b);

		copy_matrix(source_r, img->red, rws, cls);
		copy_matrix(source_g, img->green, rws, cls);
		copy_matrix(source_b, img->blue, rws, cls);

		while (rotations) {
			rotate_mx(&source_r, rws, cls, direction);
			rotate_mx(&source_g, rws, cls, direction);
			rotate_mx(&source_b, rws, cls, direction);

			swap(&rws, &cls);
			rotations--;
		}

		new->rws = rws;
		new->cls = cls;

		new->red = alloc_matrix(rws, cls);
		check(new->red);
		copy_matrix(new->red, source_r, rws, cls);

		new->green = alloc_matrix(rws, cls);
		check(new->green);
		copy_matrix(new->green, source_g, rws, cls);

		new->blue = alloc_matrix(rws, cls);
		check(new->blue);
		copy_matrix(new->blue, source_b, rws, cls);

		free_matrix(source_r, rws);
		free_matrix(source_g, rws);
		free_matrix(source_b, rws);
	}
	return new;
}

void rotate_cmd(image_t **img, int *coord, char *cmd_string, int *rotated)
{
	char copy[NMAX];
	strcpy(copy, cmd_string);
	char *word = strtok(copy, " ");
	word = strtok(NULL, " ");

	/*n acest moment word contine al doilea cuvant din comanda si
	vom test corectitudinea comenzii*/
	int len = strlen(word);
	for (int i = 0; i < len; ++i)
		if (isalpha(word[i])) {
			if (i == 0 && word[0] == '-') {
				continue;
			} else {
				printf("Invalid command\n");
				return;
			}
		}
	// comanda fiind corecta, verificam daca exista o imagine loaded
	if (!(*img)) {
		printf("No image loaded\n");
		return;
	}

	int angle = atoi(word);
	if (abs(angle) != 90 && abs(angle) != 180 && abs(angle) != 270 &&
		abs(angle) != 360 && abs(angle) != 0) {
		printf("Unsupported rotation angle\n");
		return;
	}

	if (!test_selection(coord, *img) &&
		(coord[1] - coord[0]) != coord[3] - coord[2]) {
		printf("The selection must be square\n");
		return;
	}

	/*pentru a realiza rotatiile de mai mult de 90 de grade vom roti matricea
	de mai multe ori cu 90 de grade astfel: 180 de grade prin 2 rotatii de 90
	270 prin 3 rotatii de 90, iar la rotatia de 360 nu vom face nimic*/
	int rotations = abs(angle) / 90;

	if (abs(angle) == 360 || abs(angle) == 0) {
		printf("Rotated %d\n", angle);
		return;
	}

	if (test_selection(coord, *img)) {
		if (angle < 0) {
			image_t *aux = rotate_image(*img, rotations, rotate_left);
			free_image(*img);
			*img = aux;
		} else {
			image_t *aux = rotate_image(*img, rotations, rotate_right);
			free_image(*img);
			*img = aux;
		}
		*rotated = 1;
	} else {
		if (angle < 0)
			rotate_selection(*img, rotations, coord, rotate_left);
		else
			rotate_selection(*img, rotations, coord, rotate_right);
	}

	printf("Rotated %d\n", angle);
}
