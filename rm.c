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
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/stat.h>

#define MAX_FILENAME_SIZE 	128
#define STAT				struct stat

/** Print the error message and exit */
void fail(char* message) {
	fprintf(stderr, "%s\n", message);
	exit(EXIT_FAILURE);
}

/** Print the help and exit */
void help(char* my_name) {
	printf("Usage: %s [-rfh] [-d dumpster] file1 [file2 ...]\n", my_name);
	printf("  -r Remove directories recursively\n");
	printf("  -f Force removal without backing up to the dumpster\n");
	printf("  -d Specify a dumpster directory\n");
	printf("  -h Display this help and exit\n");
	exit(EXIT_FAILURE);
}

/**
 * A function to allocate and build a file path from the given parts
 * 
 * @param count The number of strings to use
 */
char* build_path(int count, ...) {
	/* Initialize the variadic parameters */
	va_list size_pass, build_pass; 
	va_start(size_pass, count); va_start(build_pass, count);	
	
	/* Make the size pass to get the necessary size of the final buffer */
	int i; int path_size = 0;
	for(i = 0; i < count; i++) {
		/* Increment the size of the return, +1 for the slash */
		path_size += strlen(va_arg(size_pass, char*)) + 1;
	}
	va_end(size_pass);
	
	/* Create the path buffer and add in the first part */
	char* path = (char*)calloc(path_size, sizeof(char)); 
	strcpy(path, va_arg(build_pass, char*));
	for(i = 1; i < count; i++) {
		strcat(path, "/");
		strcat(path, va_arg(build_pass, char*));
	}
	va_end(build_pass);
	
	return path;
}

/**
 * 
 */
void move_file(const char* old, const char* new) {
	/* Access file information */
	STAT* metadata = (STAT*) malloc(sizeof(STAT));
	if(stat(old, metadata) == -1) {
		printf("%s is an invalid filepath, ignoring...\n", old);
		return;
	}
	
	/* Attempt the move, and handle the errors if not */
	if(rename(old, new) != 0) fail(strerror(errno));
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
		move_file(argv[i], dumped_path);
	}
	
	return EXIT_SUCCESS;
}
