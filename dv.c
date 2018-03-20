/**
 * dv.c
 * A utility for restoring files from the dumpster
 *
 * @author 	[TODO]
 * @since	3/12/2018
 * @version 1.0
 *
 * Changelog:
 * - 1.0 Initial commit
 */

#include "util.h"
#include "futil.h"

/** Print the help and exit */
void help(char* my_name) {
	printf("--------------- CS 4513 Project 1 - dv ---------------\n");
	printf("Usage: %s [-h] [-d dumpster] file1 [file2 ...]\n", my_name);
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
	char opt;
	while((opt = getopt(argc, argv, "hd:")) != -1) {
		switch(opt) {
			case 'h': help(argv[0]); break;
			case 'd': dumpster = optarg; break;
			
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
	
	/* For each specified file, recover */
	int i;
	for(i = optind; i < argc; i++) {
		/* Create the path for the dumpstered file */
		char* dumped_path = build_path(2, dumpster, argv[i]);
		
		/* Move the from the dumpster */
		move_file(dumped_path, argv[i], 1, NO_EXTEND);
	}

	return EXIT_SUCCESS;
}
