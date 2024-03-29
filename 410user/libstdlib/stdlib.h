#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <stddef.h>  /* For size_t, NULL */
#include <malloc.h>  /* "Listen to what the man says" */

/* start user-land only */

/* Apologies for the gcc-ism, but gcc gets angry w/o it */
void exit(int status) __attribute__((__noreturn__));

/* end user-land only */

long atol(const char *__str);
#define atoi(str) ((int)atol(str))

long strtol(const char *__p, char **__out_p, int __base);
unsigned long strtoul(const char *__p, char **__out_p, int __base);

#define RAND_MAX ((signed int)0x7fffffff)
int rand(void);
void srand(unsigned new_seed);

int abs(int val);

void qsort(void *a, size_t n, size_t es, int (*cmp)(char*, char*));

void panic(const char *, ...);

#endif
