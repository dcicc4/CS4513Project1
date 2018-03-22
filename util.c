/**
 * util.c
 * Implementations of useful utilities for CS 4513 Project 1
 *
 * @author  Tanuj Sane
 * @since	3/19/2018
 * @version 1.0
 *
 * Changelog:
 * - 1.0 Initial commit
 */

#include "util.h"

/** Print the error message and exit */
void fail_base(char* message, char* file, int line) {
	fprintf(stderr, "[%s:%d]: %s\n", file, line, message);
	exit(EXIT_FAILURE);
}

/** Wrap a call to a function to fail out if necessary, otherwise return the value */
int try(int retval) {
	if(retval < 0) fail(strerror(errno));
	return retval;
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