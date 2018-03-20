/**
 * futil.h
 * Definitions of useful file utilities for CS 4513 Project 1
 *
 * @author  Tanuj Sane
 * @since	3/19/2018
 * @version 1.0
 *
 * Changelog:
 * - 1.0 Initial commit
 */

#ifndef FUTIL_H
#define FUTIL_H

/* Required for filesystem access */
#include "util.h"

#define BLOCK_SIZE 	4096
#define NO_EXTEND	0
#define EXTEND		1

int copy_file(const char* from, const char* to);
void move_file(const char* old, const char* new, int recursive, int extend);

#endif	/* FUTIL_H */