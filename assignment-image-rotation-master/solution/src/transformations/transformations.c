#include "transformations.h"
#include <malloc.h>


struct image transform(const struct image *image, enum transformations transformation) {

    if (image->data == NULL) {
        return (struct image) {.width = image->width, .height = image->height, .data = NULL};
    }

    struct pixel *data = malloc(sizeof(struct pixel) * image->width * image->height);

    switch (transformation) {
        case ROTATE_90_COUNTER_CLOCKWISE:
            return rotate_90_counter_clockwise(image, data);
            break;
        default:
            for (size_t i = 0; i < image->width * image->height; i++) {
                data[i] = image->data[i];
            }
            return (struct image) {.width = image->width, .height = image->height, .data = data};
    }
}

