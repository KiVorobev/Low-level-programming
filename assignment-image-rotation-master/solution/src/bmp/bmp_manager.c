#include "bmp_manager.h"

bool read_header(FILE *file, struct bmp_header *header) {
    return fread(header, sizeof(struct bmp_header), 1, file);
}

uint8_t row_size_in_pixel(size_t width){
return width * sizeof(struct pixel);
}

uint8_t calculate_padding(size_t width) {
    return (4 - (row_size_in_pixel(width) % 4)) % 4;
}

static size_t get_file_size(size_t image_size) {
    return image_size + sizeof (struct bmp_header);
}

static size_t get_image_size(struct image *image) {
    return image->width * sizeof(struct pixel) + calculate_padding(image->width) * image->height;
}

struct bmp_header create_header(struct image *image) {
    return (struct bmp_header) {
            .bfType = 19778,
            .bfileSize = get_file_size(get_image_size(image)),
            .bfReserved = 0,
            .bOffBits = 54,
            .biSize = 40,
            .biWidth = image->width,
            .biHeight = image->height,
            .biPlanes = 1,
            .biBitCount = 24,
            .biCompression = 0,
            .biSizeImage = get_image_size(image),
            .biXPelsPerMeter = 0,
            .biYPelsPerMeter = 0,
            .biClrUsed = 0,
            .biClrImportant = 0
    };
}
