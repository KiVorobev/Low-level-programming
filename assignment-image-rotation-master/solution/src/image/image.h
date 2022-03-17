#ifndef LAB3_IMAGE_H
#define LAB3_IMAGE_H

#include <stddef.h>
#include <stdint.h>

struct pixel {
    uint8_t b,g,r;
};

struct image {
    size_t width, height;
    struct pixel *data;
};

struct image create_image(size_t width, size_t height);

struct pixel image_get_pixel(struct image image, size_t row, size_t column);

void image_set_pixel(struct image image, size_t row, size_t column, struct pixel pixel);

void destroy_image(struct image image);

#endif //LAB3_IMAGE_H
