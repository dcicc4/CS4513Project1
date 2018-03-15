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
#include <utime.h>
#include <dirent.h>

#define Stat				struct stat
#define UTime				struct utimbuf
#define Mode				__mode_t

#define DirEnt				struct dirent

#define BLOCK_SIZE			4096

/** Print the help and exit */
void help(char* my_name) {
	printf("------------------------------------------------------\n");
	printf("Usage: %s [-rfh] [-d dumpster] file1 [file2 ...]\n", my_name);
	printf("  -r Remove directories recursively\n");
	printf("  -f Force removal without backing up to the dumpster\n");
	printf("  -d Specify a dumpster directory\n");
	printf("  -h Display this help and exit\n");
	printf("------------------------------------------------------\n");
	exit(EXIT_FAILURE);
}

/** Print the error message and exit */
void fail(char* tag, char* message) {
	fprintf(stderr, "[%s]: %s\n", tag, message);
	exit(EXIT_FAILURE);
}

/** Wrap a call to a function to fail out if necessary, otherwise return the value */
int try(char* tag, int retval) {
	if(retval < 0) fail(tag, strerror(errno));
	return retval;
}

/**
 * A function to allocate and build a file path from the given parts, using "/" as the separator
 *
 * @param count The number of strings to use
 * @param ...	The strings, as char arrays, to compose into a path
 */
char* build_path(int count, ...) {
	/* Initialize the variadic parameters */
	va_list size_pass, build_pass;
	va_start(size_pass, count); va_start(build_pass, count);

	/* Make the size pass to get the necessary size of the final buffer */
	int i; int path_size = 0;
	for(i = 0; i < count; i++) {
		/* Increment the size of the path, +1 for the slash */
		path_size += strlen(va_arg(size_pass, char*)) + 1;
	}
	va_end(size_pass);

	/* Create the path buffer and add in the parts */
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
 * Copy the contents of a file into the file with the given name
 *
 * @param from	The original file to copy
 * @param to	The name of the file to copy to
 *
 * @return The file descriptor of the new copied file
 */
int copy_file(const char* from, const char* to) {
	/* Open/create both files */
	int fd_from = try("copy_file.open(from)", open(from, O_RDONLY));
	int fd_to = try("copy_file.open(to)", open(to, O_WRONLY | O_CREAT));

	/* Read bytes from the from file and put them in the to file */
	char buffer[BLOCK_SIZE]; int read_bytes = 0;
	do {
		read_bytes = try("copy_file.read(from)", read(fd_from, buffer, BLOCK_SIZE));
		try("copy_file.write(to)", write(fd_to, buffer, read_bytes));
	}
	while(read_bytes > 0);

	/* Free the file descriptor for the from file */
	close(fd_from);

	return fd_to;
}

/**
 * Move the given file, taking into consideration various cases:
 * - Files on the same filesystem are just moved as is
 * - Directories are moved if the recursive flag is set, ignored otherwise
 * - File on different partitions are copied and have their stat set to match the original
 *
 * @param old			The name of the file to be moved
 * @param new			The name of the file to be moved into
 * @param recursive		Flag indicating whether the use specified -r
 */
void move_file(const char* old, const char* new, int recursive) {
	printf("----------\n%s\n%s\n----------\n", old, new);
	/* Access file information */
	Stat metadata;
	if(stat(old, &metadata) == -1) {
		printf("%s is an invalid filepath, ignoring...\n", old);
		return;
	}

	/* Make sure directories are recursively moved */
	int is_directory = S_ISDIR(metadata.st_mode);
	if(is_directory && !recursive) {
		printf("%s is a directory (allow recursive removal with -r), ignoring...\n", old);
		return;
	}
  //Puts with a .1... extention
  if(access(new,F_OK)!=-1){
    int extCount = 0;
    char* tempPath = malloc(strlen(new)+sizeof(char)*3);
    strcpy(tempPath, new);
    strcat(tempPath, ".1");
  	while(access(tempPath, F_OK)!=-1 && extCount <10){
      extCount++;
      tempPath[strlen(tempPath)-1] =  extCount+'0';
    }
    if(extCount >9){
      char* message = malloc(strlen(new)+sizeof(char)*45);
      sprintf(message, "dumpster is has too many versions of a file: %s", old );
      fail("move_file.exists", message);
    }
    new = tempPath;
  }


	/* Attempt the move, and handle the errors if not */
	if(rename(old, new) != 0) {
		switch(errno) {
			/* This indicates that the file is on a different partition and must be handled differently */
			case EXDEV:
				if(is_directory) {
					try("move_file.rename().EXDEV.mkdir(new)", mkdir(new, metadata.st_mode));

					DIR* dp = opendir(old);
					if(dp == NULL) fail("move_file.rename(old,new)->EXDEV.opendir(old)", "Failed to open directory");

					DirEnt* d;
					do {
						d = readdir(dp);

						/* Ignore the special files . and .. */
						if(!strcmp(d->d_name, ".") || !strcmp(d->d_name, "..")) continue;

						/* Generate the names for the files within the dir and recursively move them */
						char* old_new = build_path(2, old, d->d_name);
						char* new_new = build_path(2, new, d->d_name);
						move_file(old_new, new_new, recursive);

					}
					while(d);
				}
				else {
					copy_file(old, new);

					/* Set the mod and access times to match the old file */
					UTime times; times.actime = metadata.st_atime; times.modtime = metadata.st_mtime;
					try("move_file.rename(old,new)->EXDEV.utime", utime(new, &times));

					/* Set the permissions to match the old file */
					Mode ugo = metadata.st_mode;
					try("move_file.rename(old,new)->EXDEV.chmod", chmod(new, ugo));

					/* Remove the old file */
					try("remove the old file", remove(old));
				}

				break;

			/* Otherwise just give up and fail out*/
			default: try("move_file.rename(old,new)", -1); break;
		}
	}
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
				if(optopt == 'd') fail("main.getopt()", "Must specify a dumpster directory with the -d option");
				else printf("Unrecognized option -%c, ignoring...\n", optopt);
				break;
		}
	}


	/* Grab the DUMPSTER environment variable and fail out if not set */
	if(dumpster == NULL) dumpster = getenv("DUMPSTER");
	if(dumpster == NULL) fail("main.getenv()", "Dumpster directory not found; set the DUMPSTER environment variable or specify a dumpster with -d");

	/* For each specified file, it's trash day! */
	int i;
	for(i = optind; i < argc; i++) {
		/* Create the path for the dumpstered file */
		char* dumped_path = build_path(2, dumpster, argv[i]);

		/* For forced removal, just delete the file */
		if(force) try("main.remove()", remove(argv[i]));

		/* Otherwise move the file to the dumpster */
		else move_file(argv[i], dumped_path, recursive);
	}

	return EXIT_SUCCESS;
}
