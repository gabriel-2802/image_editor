# MY_IMAGE_EDITOR - Copyright Carauleanu Valentin Gabriel 311CA

My third programming homework.

# Project Overview

This project uses three static libraries:
- `dynamic_alloc.h`: Handles dynamic memory allocation, reallocation
- `aux_functions.h`: Contains auxiliary functions
- `editing_tools.h`: Incorporates the main operations of the `image_editor` program, using functions from the other libraries

Given the necessity to work with large image matrices, image structures and their matrices are dynamically allocated and freed when a new image is loaded. Two data types are defined: `image_t` and `rotate`. `image_t` stores image information, using three matrices. . `rotate` is used to define pointers to the `rotate_left` and `rotate_right` functions.

In the main function, a command is transformed into a numerical value, which is used in a switch instruction for better organization.

## Operations
- **Load**: Before reading from a file, any existing image is cleared from memory and the `image_t` pointer is initialized to `NULL`. `comment_case` function is used to ignore comment lines and read image's descriptive data. For a binary file, the cursor's position is saved with `fgetpos`, the file is closed, reopened in "rb" mode, and cursor is set to the initial position using `fsetpos`.

- **Select**: A coordinate vector stores x1, x2, y1, y2 values, where x represents image width and y represents image height. A full image selection is applied after the rotate, crop, and load operations.

- **Crop**: A new image is created using the submatrix delimited by the coordinate vector. If the selection is full, the crop operation will do nothing.

- **Apply**: Only color images are targeted. The image selection defined by the coordinate vector is traversed, ignoring edge elements. Inner elements are modified using the kernel matrix and a 3x3 submatrix from the image matrix, centered on the current element. The same `apply` function is used for any effect.

- **Save**: Binary files contain both ASCII and binary elements. Initially, the file is opened in "rt" mode to write the image's descriptive elements. The file is then closed, cursor's position is saved using `fgetpos`, reopened in "rb" mode, and cursor is returned to its initial position using `fsetpos`. After that, the image matrices are written in binary.

- **Rotate**: Successive 90-degree rotations are used to achieve any permissible rotation. If the selection is full, a new `image_t` structure containing the rotated image and potential new dimensions is created. For incomplete selections, the selected submatrix is copied into an auxiliary matrix that is subsequently rotated. Finally, the selected submatrix in the image is overwritten with values from the auxiliary matrix.
