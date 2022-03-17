#ifndef LAB3_FILE_MANAGER_H
#define LAB3_FILE_MANAGER_H

#include "../util/status_list.h"
#include <stdio.h>

struct maybe_file {
    enum status status;
    FILE *file;
};

struct maybe_file file_open(const char *file_name, const char *mode);

enum status file_close(FILE *file);

#endif //LAB3_FILE_MANAGER_H
