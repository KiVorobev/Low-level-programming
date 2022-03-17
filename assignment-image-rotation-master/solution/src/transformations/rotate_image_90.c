#include "rotate_image_90.h"

struct image rotate_90_counter_clockwise(const struct image *image, struct pixel *data) {

    for (size_t i = 0; i < image->height; ++i) {
        for (size_t j = 0; j < image->width; ++j) {
            data[image->height * j + (image->height - i - 1)] = image->data[i * image->width + j];
        }
    }

    return (struct image) {.width = image->height, .height = image->width, .data = data};

}

