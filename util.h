/**
 * util.h
 * Definitions of useful utilities for CS 4513 Project 1
 *
 * @author  Tanuj Sane
 * @since	3/19/2018
 * @version 1.0
 *
 * Changelog:
 * - 1.0 Initial commit
 */

#ifndef UTIL_H
#define UTIL_H

/* Required for filesystem access */
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

/* Typedefs to make things prettier */
#define Stat				struct stat
#define UTime				struct utimbuf
#define Mode				__mode_t
#define DirEnt				struct dirent

void fail(char* message);
int try(int retval);
char* build_path(int count, ...);

#endif	/* UTIL_H */