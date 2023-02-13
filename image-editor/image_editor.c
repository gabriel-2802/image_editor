//copyright Carauleanu Valentin Gabriel 311CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "aux_functions.h"
#include "dynamic_alloc.h"
#include "editing_tools.h"
#define NMAX 100
#define SIZE 4
#define MAXPIXEL 256
#define K 3

int main(void)
{
	image_t *img = NULL;
	char *cmd_string = NULL;
	/*cmd retine in memorie valorea numerica corespunzatoare
	cmd_string-ului actual, coord va retine selectia curenta a
	imaginii, iar rotated va deveni 1 in cazul unei rotatii
	complete a imaginii incarcate in memorie*/
	int cmd = 0, done = 0, coord[SIZE] = {0};

	int rotated = 0;

	while (1) {
		cmd_string = read_cmd();
		if (!cmd_string) {
			printf("Invalid command\n");
			continue;
		}
		command(&cmd, cmd_string);

		switch (cmd) {
		case 1:
			initialise_img(&img);
			load_cmd(cmd_string, &img);
			if (img)
				select_all(coord, img);
			break;
		case 2:
			select_cmd(cmd_string, img, coord);
			break;
		case 3:
			select_all_cmd(img, coord);
			break;
		case 4:
			histogram_cmd(img, cmd_string);
			break;
		case 5:
			equalize_cmd(img);
			break;
		case 6:
			crop_cmd(&img, coord);
			if (img)
				select_all(coord, img);
			break;
		case 7:
			apply_cmd(&img, coord, cmd_string);
			break;
		case 8:
			save_cmd(img, cmd_string);
			break;
		case 9:
			if (!img)
				printf("No image loaded\n");
			done = 1;
			break;
		case 10:
			rotate_cmd(&img, coord, cmd_string, &rotated);
			if (rotated) {
				select_all(coord, img);
				rotated = 0;
			}
			break;
		default:
				printf("Invalid command\n");
			break;
		}

		if (cmd_string)
			free(cmd_string);
		if (done)
			break;
	}

	if (img)
		free_image(img);

	return 0;
}
