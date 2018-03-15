#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <dirent.h>

/** Print the error message and exit */
void fail(char* message) {
	fprintf(stderr, "%s\n", message);
	exit(EXIT_FAILURE);
}

/** Print the help and exit */
void help(char* my_name) {
	printf("Usage: %s [-dh]\n", my_name);
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


void dump_file(const char* dump_path){
	DIR *dump;
	dump = opendir(dump_path);
	struct dirent *dump_file;
	char* file_path;
	while ((dump_file = readdir(dump)) != NULL){
		//printf("found %s, %d, %d\n",dump_file->d_name,dump_file->d_type,strcmp(dump_file->d_name,".."));
		if(dump_file->d_type == DT_REG || dump_file->d_type == DT_DIR){
			if(strcmp(dump_file->d_name,"..") != 0 && strcmp(dump_file->d_name,".") != 0){
				file_path = build_path(2, dump_path, dump_file->d_name);
				if(remove(file_path) != 0) fail(strerror(errno));
				printf("deleted %s\n",dump_file->d_name);
			}
		}
	}
	printf("Removed all files in %s\n", dump_path);
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
	
	dump_file(dumpster);
	return EXIT_SUCCESS;
}
