/**
 * rm.c
 * A replacement utility for file removal that puts removed files into a dumpster
 *
 * @author 	Tanuj Sane
 * @since	3/12/2018
 * @version 1.0
 *
 * Changelog:
 * - 1.0 Initial commit
 */

#include "util.h"
#include "futil.h"

#define BLOCK_SIZE			4096

/** Print the help and exit */
void help(char* my_name) {
	printf("--------------- CS 4513 Project 1 - rm ---------------\n");
	printf("Usage: %s [-rfh] [-d dumpster] file1 [file2 ...]\n", my_name);
	printf("  -r Remove directories recursively\n");
	printf("  -f Force removal without backing up to the dumpster\n");
	printf("  -d Specify a dumpster directory\n");
	printf("  -h Display this help and exit\n");
	printf("------------------------------------------------------\n");
	exit(EXIT_FAILURE);
}

/** MAIN */
int main(int argc, char** argv) {
	/* Verify and parse commandline arguments */
	if(argc < 2) help(argv[0]);

	/* Set up configuration from commandline flags */
	opterr = 0;
	char* dumpster = NULL;
	char opt; int recursive = 0, force = 0;
	while((opt = getopt(argc, argv, "rfhd:")) != -1) {
		switch(opt) {
			case 'r': recursive = 1; break;
			case 'f': force = 1; break;
			case 'd': dumpster = optarg; break;
			case 'h': help(argv[0]); break;

			/* This happens if an unexpected argument shows up */
			case '?':
				/* If the argument was -d, the argument was left out */
				if(optopt == 'd') fail("Must specify a dumpster directory with the -d option");
				else printf("Unrecognized option -%c, ignoring...\n", optopt);
				break;
		}
	}

	/* Grab the DUMPSTER environment variable and fail out if not set */
	if(dumpster == NULL) dumpster = getenv("DUMPSTER");
	if(dumpster == NULL) fail("Dumpster directory not found; set the DUMPSTER environment variable or specify a dumpster with -d");

	/* For each specified file, it's trash day! */
	int i;
	for(i = optind; i < argc; i++) {
		/* Create the path for the dumpstered file */
		char* dumped_path = build_path(2, dumpster, argv[i]);

		/* For forced removal, just delete the file */
		if(force) try(remove(argv[i]));

		/* Otherwise move the file to the dumpster */
		else move_file(argv[i], dumped_path, recursive, EXTEND);
	}

	return EXIT_SUCCESS;
}
