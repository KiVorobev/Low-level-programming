#ifndef LAB3_TRANSFORMATIONS_H
#define LAB3_TRANSFORMATIONS_H

#include "../image/image.h"
#include "rotate_image_90.h"
#include <stddef.h>

enum transformations {
    ROTATE_90_COUNTER_CLOCKWISE = 0
};

struct image transform(const struct image *image, enum transformations transformation);


#endif //LAB3_TRANSFORMATIONS_H
