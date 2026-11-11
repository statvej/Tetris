#include "storage.h"

#include <stdio.h>
#include <stdlib.h>

bool storage_path(char *buf, size_t size, const char *name)
{
    const char *dir = getenv("TETRIS_DATA_DIR");
    if (!dir || !*dir)
        dir = getenv("HOME");
    if (!dir || !*dir)
        dir = ".";
    int n = snprintf(buf, size, "%s/.tetris_%s", dir, name);
    return n > 0 && (size_t)n < size;
}
