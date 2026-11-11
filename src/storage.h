#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>
#include <stddef.h>

/*
 * Build the path of a data file. Files live in $TETRIS_DATA_DIR if set,
 * otherwise $HOME, otherwise the current directory, as ".tetris_<name>".
 * Returns false if the path does not fit in buf.
 */
bool storage_path(char *buf, size_t size, const char *name);

#endif
