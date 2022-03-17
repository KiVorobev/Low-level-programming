#include "bmp_converter.h"

enum status from_bmp(FILE *input_file, struct image *image) {
    struct bmp_header header;
    if (!read_header(input_file, &header)) return READ_ERROR;

    *image = create_image(header.biWidth, header.biHeight);

    const uint8_t padding = calculate_padding(header.biWidth);

    for (size_t i = 0; i < image->height; i++) {
        for (size_t j = 0; j < image->width; j++) {
        	void * const start_index = &(image->data[image->width * i + j]);
        	fread(start_index, sizeof(struct pixel), 1, input_file);
        	}
        fseek(input_file, padding, SEEK_CUR);
    	}
    return OK;
}

enum status to_bmp(FILE *output_file, struct image *image) {
    struct bmp_header header = create_header(image);
    if (!fwrite(&header, sizeof(struct bmp_header), 1, output_file)) return WRITE_ERROR;

    fseek(output_file, header.bOffBits, SEEK_SET);

    const uint8_t zero_line = 0;

    if (image->data == NULL) return WRITE_ERROR;
    for (size_t i = 0; i < image->height; i++) {
    	void * const start_index = image->data + i * image->width;
    	size_t const size = image->width * sizeof(struct pixel);
    	fwrite(start_index, size, 1, output_file);
    	for (size_t j = 0; j < calculate_padding(image->width); j++) {
    		fwrite(&zero_line, 1, 1, output_file);
    		}
    	}
    return OK;
}

