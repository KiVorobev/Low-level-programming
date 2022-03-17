#include "status_manager.h"

typedef char *(*error_text)(enum status);

static const char *const error_message[] = {
        [OK] = "The program has worked successfully! :)",
        [OPEN_ERROR] = "Couldn't open the file! :(",
        [CLOSE_ERROR] = "Failed to close file! :(",
        [READ_ERROR] = "Couldn't read the file! :(",
        [WRITE_ERROR] = "Failed to write to file! :(" 
};

void print_status(enum status status) {
    printf("-->%s\n", error_message[status]);
}

