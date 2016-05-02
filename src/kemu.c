#include "rom.h"
#include "memory.h"
#include "cpu.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BOOTSTRAP_SIZE	0xFF

static void usage(void);

static bool verbose = false;

int main(int argc, char **argv)
{
	int c, i = 0;
	FILE *romfile, *bootstrap_f;
	char *bootstrap;

	if (argc < 2) {
		usage();
		exit(1);
	}

	/* Parse the command line switches. */
	while ((c = getopt(argc, argv, "hv")) != -1) {
		switch (c) {
		case 'h':             /* Print a help message. */
			usage();
			break;
		case 'v':             /* Be verbose throughout the emulation. */
			verbose = true;
			break;
		default:
			usage();
			exit(1);
		}
	}

	/* Load the bootstrap. */
	bootstrap_f = fopen("bootstrap.bin", "r");
	if (!bootstrap_f) {
		fprintf(stderr, "ERROR: failed to load bootstrap\n");
		exit(1);
	}
	bootstrap = malloc(sizeof(char) * BOOTSTRAP_SIZE);
	while ((c = fgetc(bootstrap_f)) != EOF) {
		if (verbose)
			printf("kemu: loaded bootstrap byte %02X\n", c);
		bootstrap[i++] = c;
	}

	/* Bootstrap program is 256 bytes exactly. */
	if (i < BOOTSTRAP_SIZE) {
		fprintf(stderr, "ERROR: bootstrap is less than %d bytes\n", BOOTSTRAP_SIZE);
		exit(1);
	}

	/* Initialize the ROM. */
	if (verbose)
		printf("kemu: initializing ROM\n");
	romfile = fopen(argv[argc - 1], "r");
	if (!romfile) {
		fprintf(stderr, "ERROR: romfile %s not found\n", argv[argc - 1]);
		exit(1);
	}
	init_rom(romfile, verbose, true);

	/* Initialize the memory system and load the bootstrap into memory. */
	if (verbose)
		printf("kemu: initializing memory\n");
	minit(verbose);
	mput_region((char *)0x0, bootstrap, BOOTSTRAP_SIZE, true);

	/* Load the ROM into memory. */
	if (verbose)
		printf("kemu: loading bootstrap into memory\n");
	mput_region((char *)ROM_ENTRY_POINT, get_rom(), get_romlen(), true);
	close_rom();

	/* Initialize the CPU and begin execution. */
	if (verbose)
		printf("kemu: initializing CPU and beginning execution\n");
	init_cpu(verbose);
	begin_execution();

	/* Control will never reach here. */
	return 1;
}

static void usage(void)
{
	printf("Usage: kemu [-v] [-h] [romfile]\n");
}