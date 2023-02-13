#copyright Carauleanu Valentin Gabriel 311CA

CC=gcc
CFLAGS=-Wall -Wextra -std=c99

build:
	$(CC) $(CFLAGS) image_editor.c aux_functions.c dynamic_alloc.c editing_tools.c -lm -o image_editor
clean:
	rm -f image_editor

.PHONY: pack clean
