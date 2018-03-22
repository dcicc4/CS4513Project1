/**
 * export.h
 * This file imports all the necessary functions for testing
 *
 * @author 	Tanuj Sane
 * @since	3/12/2018
 * @version 1.0
 *
 */

#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

void rm(int nfiles, char** filenames, int recursive, int force, char* dumpster);
void dv(int nfiles, char** filenames, char* dumpster);
void dump(const char* dumpster);

#endif	/* EXPERIMENTS_H */