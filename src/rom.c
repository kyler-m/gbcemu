#include "rom.h"

#include <stdlib.h>

static char *rom = NULL;
static int romlen = 0;
static int cur_byte = 0;
static FILE *romfile;

void init_rom(FILE *fp, int mem_load, int verbose_load)
{
	romfile = fp;

	if (mem_load) {
		int count = 0;
		int c, i = 0;

		while ((c = fgetc(fp)) != EOF && ++count)
			;

		if (verbose_load)
			printf("rom: loaded rom is %d bytes\n", count);

		rom = malloc(sizeof(char) * count);

		fseek(fp, -count, SEEK_CUR);

		while (((c = fgetc(fp)) != EOF))
			rom[i++] = c;

		romlen = i - 1;

		fclose(romfile);
	}
}

void close_rom(void)
{
	if (rom)
		free(rom);
	else
		fclose(romfile);
}

char *get_rom(void)
{
	return rom;
}

int get_romlen(void)
{
	return romlen;
}

int get_byte(void)
{
	if (rom)
		return (cur_byte < romlen) ? rom[cur_byte++] : -1;
	else
		return fgetc(romfile);
}