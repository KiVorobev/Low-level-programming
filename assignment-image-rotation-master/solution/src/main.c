#include "bmp/bmp_converter.h"
#include "file_manager/file_manager.h"
#include "transformations/transformations.h"
#include "util/status_manager.h"
#include <stdlib.h>

int main( int argc, char** argv ) {
    if (argc != 3) {
        perror("Pass all the required, please.\n");
        return 0;
    }

    const char *input_file_name = argv[1];
    const char *output_file_name = argv[2];

    struct maybe_file input_file = file_open(input_file_name, "r");

    if (input_file.status != OK) {
    print_status(OPEN_ERROR);
    abort();
    }

    struct image input_image;

    if (from_bmp(input_file.file, &input_image) != OK) {
    print_status(READ_ERROR);
    abort();
    }

    if (file_close(input_file.file) != OK) {
    print_status(CLOSE_ERROR);
    abort();
    }

    struct image output_image = transform(&input_image, ROTATE_90_COUNTER_CLOCKWISE);
    destroy_image(input_image);

    struct maybe_file output_file = file_open(output_file_name, "w");
    
    if (output_file.status != OK) {
    print_status(OPEN_ERROR);
    abort();
    }

    if (to_bmp(output_file.file, &output_image) != OK) {
    print_status(WRITE_ERROR);
    abort();
    }

    if (file_close(output_file.file) != OK) {
    print_status(CLOSE_ERROR);
    abort();
    }

    destroy_image(output_image);
    
    print_status(OK);
    return 0;
}

