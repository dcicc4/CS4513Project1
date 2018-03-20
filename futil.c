/**
 * futil.c
 * Implementations of useful file utilities for CS 4513 Project 1
 *
 * @author  Tanuj Sane
 * @since	3/19/2018
 * @version 1.0
 *
 * Changelog:
 * - 1.0 Initial commit
 */

#include "util.h"
#include "futil.h"

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
	int fd_from = try(open(from, O_RDONLY));
	int fd_to = try(open(to, O_WRONLY | O_CREAT));

	/* Read bytes from the from file and put them in the to file */
	char buffer[BLOCK_SIZE]; int read_bytes = 0;
	do {
		read_bytes = try(read(fd_from, buffer, BLOCK_SIZE));
		try(write(fd_to, buffer, read_bytes));
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
 * @param extend		Flag indicating whether name collisions should be resolved with a .N extension
 */
void move_file(const char* old, const char* new, int recursive, int extend) {
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
	
	int worked = access(new, F_OK); 
	if(!extend && worked == 0) {
		printf("%s already exists here, ignoring...\n", old);
		return;
	}	
	
	/* Create the extended filename */
	int cp_counter = 0;
	char* temp_new = malloc(strlen(new) + 3 * sizeof(char));
	strcpy(temp_new, new);
	while(worked == 0) {
		if(++cp_counter > 9) {
			printf("%s has too many versions in the dumpster, ignoring...\n", old);
			return;
		}
		
		/* Add the counter extension and test */
		strcpy(temp_new, new);
		temp_new[strlen(new)] = '.';
		temp_new[strlen(new) + 1] = '0' + cp_counter;
		temp_new[strlen(new) + 2] = '\0';
		
		worked = access(temp_new, F_OK);
	}
	
	/* Reassign the new pointer to the extended filename */
	new = temp_new;

	/* Attempt the move, and handle the errors if not */
	if(rename(old, new) != 0) {
		switch(errno) {
			/* This indicates that the file is on a different partition and must be handled differently */
			case EXDEV:
				if(is_directory) {
					try(mkdir(new, metadata.st_mode));

					DIR* dp = opendir(old);
					if(dp == NULL) fail("Failed to open directory");

					DirEnt* d = readdir(dp);
					while(d) {
						if(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) {
							d = readdir(dp);
							continue;
						}
						
						/* Generate the names for the files within the dir and recursively move them */
						char* old_new = build_path(2, old, d->d_name);
						char* new_new = build_path(2, new, d->d_name);
						move_file(old_new, new_new, recursive, extend);
						
						d = readdir(dp);
					}
					
					/* Deallocate and remove directory */
					closedir(dp); rmdir(old);
					
				}
				else {
					copy_file(old, new);

					/* Set the mod and access times to match the old file */
					UTime times; times.actime = metadata.st_atime; times.modtime = metadata.st_mtime;
					try(utime(new, &times));

					/* Set the permissions to match the old file */
					try(chmod(new, metadata.st_mode));

					/* Remove the old file */
					try(remove(old));
				}
				break;

			/* Otherwise just give up and fail out*/
			default: fail(strerror(errno));
		}
	}
}