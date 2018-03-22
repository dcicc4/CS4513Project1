/**
 * dump.c
 * A utility for removing all files from the dumpster
 *
 * @author 	[TODO]
 * @since	3/12/2018
 * @version 1.0
 *
 * Changelog:
 * - 1.0 Initial commit
 */

#include "util.h"
#include "export.h"

/**
 * The file restoration funtion containing all arguments, implemented like this so that the functionality can be exported to the timing code
 *
 * @param dumpster	The path to the dumpster
 */
void dump(const char* dumpster){
	DIR *trash; trash = opendir(dumpster);
	DirEnt *dump_file;
	char* file_path;
	
	while ((dump_file = readdir(trash)) != NULL){
		file_path = build_path(2, dumpster, dump_file->d_name);
		
		if(dump_file->d_type == DT_REG) try(remove(file_path));
		else if(dump_file->d_type == DT_DIR){
			if(strcmp(dump_file->d_name,"..") != 0 && strcmp(dump_file->d_name,".") != 0){
				dump(file_path);
				try(remove(file_path));
			}
		}
	}
}

#ifndef AS_LIB
/** Print the help and exit */
void help(char* my_name) {
	printf("-------------- CS 4513 Project 1 - dump --------------\n");
	printf("Usage: %s [-h] [-d dumpster] file1 [file2 ...]\n", my_name);
	printf("  -d Specify a dumpster directory\n");
	printf("  -h Display this help and exit\n");
	printf("------------------------------------------------------\n");
	exit(EXIT_FAILURE);
}

/** MAIN */
int main(int argc, char** argv){
	/* Set up configuration from commandline flags */
	opterr = 0;	
	char* dumpster = NULL;
	char opt;
	while((opt = getopt(argc, argv, "hd:")) != -1) {
		switch(opt) {
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

	if(dumpster == NULL) dumpster = getenv("DUMPSTER");
	if(dumpster == NULL) fail("Dumpster directory not found; set the DUMPSTER environment variable or specify a dumpster with -d");
	
	dump(dumpster);
	return EXIT_SUCCESS;
}
#endif
