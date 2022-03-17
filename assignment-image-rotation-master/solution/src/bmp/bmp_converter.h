#ifndef LAB3_BMP_CONVERTER_H
#define LAB3_BMP_CONVERTER_H

#include "../image/image.h"
#include "../util/status_list.h"
#include "bmp_manager.h"
#include <stdio.h>

enum status from_bmp(FILE *input_file, struct image *image);

enum status to_bmp(FILE *output_file, struct image *image);

#endif //LAB3_BMP_CONVERTER_H
