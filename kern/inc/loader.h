/* The 15-410 kernel project
 *
 *     loader.h
 *
 * Structure definitions, #defines, and function prototypes
 * for the user process loader.
 */

#ifndef _LOADER_H
#define _LOADER_H

/* --- Prototypes --- */

int getbytes(const char *filename, int offset, int size, char *buf);
int find_app_index(const char *filename);
int init_address_space(const char *filename);

/*
 * Declare your loader prototypes here.
 */

#endif /* _LOADER_H */
