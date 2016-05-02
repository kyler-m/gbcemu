#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>

#define MEMORY_SIZE 0xFFFF

void minit(bool);
void mclose(void);
void mget(void *, void *, int);
void mput(char *, char, bool);
void mput_region(char *, char *, int, bool);

#endif