#ifndef LAB3_BMP_MANAGER_H
#define LAB3_BMP_MANAGER_H

#include "../image/image.h"
#include "bmp_header.h"
#include <stdbool.h>
#include <stdio.h>

bool read_header(FILE *file, struct bmp_header *header);

uint8_t calculate_padding(size_t width);

struct bmp_header create_header(struct image *image);

#endif //LAB3_BMP_MANAGER_H
