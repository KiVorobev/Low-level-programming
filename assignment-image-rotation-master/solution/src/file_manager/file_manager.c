#include "file_manager.h"

struct maybe_file file_open(const char *file_name, const char *mode) {
    FILE *file;
    if ((file = fopen(file_name, mode)) == NULL) return (struct maybe_file) {.status = OPEN_ERROR};
    else return (struct maybe_file) {.status = OK, .file = file};
}

enum status file_close(FILE *file) {
    if (fclose(file) == EOF) return CLOSE_ERROR;
    else return OK;
}

