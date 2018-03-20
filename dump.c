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

/** Print the help and exit */
void help(char* my_name) {
	printf("-------------- CS 4513 Project 1 - dump --------------\n");
	printf("Usage: %s [-h] [-d dumpster] file1 [file2 ...]\n", my_name);
	printf("  -d Specify a dumpster directory\n");
	printf("  -h Display this help and exit\n");
	printf("------------------------------------------------------\n");
	exit(EXIT_FAILURE);
}

/**
 * A function to delete all files and directories from the given path
 *
 * @param dump_path	The path to the dumpster directory from which all files will be deleted
 */
void dump_all(const char* dump_path){
	DIR *dump;
	dump = opendir(dump_path);
	DirEnt *dump_file;
	char* file_path;
	while ((dump_file = readdir(dump)) != NULL){
		file_path = build_path(2, dump_path, dump_file->d_name);
		if(dump_file->d_type == DT_REG) try(remove(file_path));
		else if(dump_file->d_type == DT_DIR){
			if(strcmp(dump_file->d_name,"..") != 0 && strcmp(dump_file->d_name,".") != 0){
				dump_all(file_path);
				try(remove(file_path));
			}
		}
	}
}

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
	
	dump_all(dumpster);
	return EXIT_SUCCESS;
}
