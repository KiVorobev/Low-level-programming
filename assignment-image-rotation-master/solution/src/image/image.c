#include "image.h"
#include <malloc.h>

struct image create_image (size_t width, size_t height) {
    return (struct image) {.width = width, .height = height,
            .data = malloc(width * height * sizeof(struct pixel))};
}

struct pixel image_get_pixel(struct image image, size_t row, size_t column){
    return image.data[row * image.width + column];
}

void image_set_pixel(struct image image, size_t row, size_t column, struct pixel pixel){
    image.data[row * image.width + column] = pixel;
}

void destroy_image(struct image image){
    free(image.data);
}
