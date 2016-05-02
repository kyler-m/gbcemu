#include "memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *translate_ptr(char *);

static bool verbose = false;
static char *memory = NULL;

void minit(bool verbose_mem)
{
	verbose = verbose_mem;

	memory = malloc(sizeof(char) * MEMORY_SIZE);

	if (verbose)
		printf("mem: memory system initialized, memory beginning at %p, ending at %p\n", memory, memory + MEMORY_SIZE);
}

static char *translate_ptr(char *ptr)
{
	if (verbose)
		printf("mem: translating %p to ", ptr);
	ptr += (long)memory;
	if (verbose)
		printf("%p\n", ptr);
	return ptr;
}

void mclose(void)
{
	if (memory)
		free(memory);
}

void mget(void *dest, void *src, int nbytes)
{
	src = translate_ptr(src);

	memcpy(dest, src, nbytes);
}

void mput(char *ptr, char val, bool translate)
{
	if (translate)
		ptr = translate_ptr(ptr);
}

void mput_region(char *ptr, char *src, int nbytes, bool translate)
{
	if (translate)
		ptr = translate_ptr(ptr);

	if (verbose) {
		int i;

		printf("mem: src pointer is at %p\n", src);

		for (i = 0; i < nbytes; i++) {
			//printf("mem: moving %02X\n", (unsigned char)src[i]);
			ptr[i] = src[i];
		}
	} else {
		memcpy(ptr, src, nbytes);
	}
}