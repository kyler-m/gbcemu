#ifndef ROM_H
#define ROM_H

#include <stdio.h>

#define ROM_ENTRY_POINT 0x100

void init_rom(FILE *, int, int);
void close_rom(void);
int get_byte(void);
char *get_rom(void);
int get_romlen(void);

#endif