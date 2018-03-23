/**
 * experiments.c
 * Test for the program
 *
 * @author 	Drew Ciccarelli, Tanuj Sane
 * @since	3/12/2018
 * @version 1.0
 *
 * Changelog:
 * - 1.0 Initial commit
 */

#include <sys/time.h>
#include <time.h>

#include "util.h"
#include "futil.h"
#include "export.h"

#define TimeVal struct timeval

const char* testing_directory = "testing_directory";
// TODO: This is specfic to Tanuj's machine with a flash drive mounted to that location, need to make not dependent on this
const char* separate_fs_dir = "/mnt/usb";
char test_file[] = "00.test";

/**
 * Set up a testing directory with 20 text files
 */
void setup() {
	try(mkdir(testing_directory, 0755));
	int i;
	for(i = 1; i < 21; i++) {
		test_file[0] = '0' + (i / 10);
		test_file[1] = '0' + (i % 10);
		open(build_path(2, testing_directory, test_file), O_CREAT, 0555);
	}
}

void teardown() {
	dump(testing_directory);
	remove(testing_directory);
}

double get_time() {
	TimeVal* time = (TimeVal*) malloc(sizeof(TimeVal));
	gettimeofday(time, NULL);
	return ((double)time->tv_sec + ((double)time->tv_usec)/1000000);
}

double time_rename() {
	setup();

	/* Run 20 renaming operations and collect the values */
	int i; double sum = 0; char moved_file[] = "00.t3st";
	for(i = 1; i < 21; i++) {
		test_file[0] = '0' + (i / 10); moved_file[0] = '0' + (i / 10);
		test_file[1] = '0' + (i % 10); moved_file[1] = '0' + (i % 10);
    
		char* firstPath = build_path(2, testing_directory, test_file);
    	char* secondPath =  build_path(2, testing_directory, moved_file);
    	sync();
    	
		double start = get_time();
		rename(firstPath,secondPath);
		sync();
		double end = get_time();
		
		printf("Rename %d: %f\n", i, end - start);
		sum += (end - start);
	}

	teardown();

	return sum / 20.0;
}

double time_link_unlink() {
	setup();

	/* Run 20 link + unlink operations and collect the values */
	int i; double sum = 0; char moved_file[] = "00.csv";
	for(i = 1; i < 21; i++) {
		test_file[0] = '0' + (i / 10); moved_file[0] = '0' + (i / 10);
		test_file[1] = '0' + (i % 10); moved_file[1] = '0' + (i % 10);
    	
		char* old = build_path(2, testing_directory, test_file);
    	char* new =  build_path(2, testing_directory, moved_file);
    	sync();
    
		double start = get_time();
    	link(old, new);
		unlink(old);
		sync();
		double end = get_time();
		
		printf("Link + Unlink %d: %f\n", i, end - start);
		sum += (end - start);
	}

	teardown();

	return sum / 20.0;
}

double time_copy_file() {
	try(mkdir(testing_directory, 0755));
	
	srand(time(NULL));
	char* original = build_path(2, separate_fs_dir, "original.test");
	const long original_size = 1000 * BLOCK_SIZE;

	/* Open the file and populate with garbage */
	int fd_from = try(open(original, O_WRONLY | O_CREAT));
	long i; char contents[original_size];
	for(i = 0; i < original_size; i++) {
		contents[i] = 'A' + (rand() % 26);
	}
	try(write(fd_from, contents, original_size));
	sync();

	/* Copy the file */
	int sum;
	for(i = 1; i < 21; i++) { 
		test_file[0] = '0' + (i / 10);
		test_file[1] = '0' + (i % 10);
		
		double start = get_time();
		copy_file(original, test_file);
		sync();
		double end = get_time();
		
		/* Get the size of the file */
		Stat metadata; try(stat(test_file, &metadata));
		long file_size = metadata.st_size;
		
		/* Grab data for successful copies */
		if(file_size == original_size){
			printf("%lu: Copied %lu bytes in %f seconds\n", i, file_size, end - start);
			sum += (end - start);
		}
	}

	double time_avg = sum / 20.0;
	
	teardown();
	
	return ((double) original_size) / time_avg;
}



/**
 * Times the Copying of the contents of a file into the file with the given name
 *
 * @param from	The original file to copy
 * @param to	The name of the file to copy to
 *
 * @return The bps for the file copy
 */
int timeCP(const char* from, const char* to) {
	/* Open/create both files */
  TimeVal* startTime= (TimeVal*) malloc(sizeof(TimeVal));
  TimeVal* endTime= (TimeVal*) malloc(sizeof(TimeVal));
	int fd_from = try(open(from, O_RDONLY));
	int fd_to = try(open(to, O_WRONLY | O_CREAT));
  double bps = 0;
  int loopCounter = 0;
	/* Read bytes from the from file and put them in the to file */
	char buffer[BLOCK_SIZE]; int read_bytes = 0;
	do {
    sync();
    gettimeofday(startTime, NULL);
		read_bytes = try(read(fd_from, buffer, BLOCK_SIZE));
		try(write(fd_to, buffer, read_bytes));
    sync();
    gettimeofday(endTime, NULL);
    double diff = ((double)endTime->tv_sec +((double)endTime->tv_usec)/1000000)- ((double)startTime->tv_sec+ ((double)startTime->tv_usec)/1000000);
    bps += read_bytes/diff;
    loopCounter++;
	}
	while(read_bytes > 0);
  bps = bps/loopCounter;
	/* Free the file descriptor for the from file */
	close(fd_from);

	return bps;
}

/** MAIN */
int main(){
  /*double total = 0;
  for (int i =0; i < 20; i++){
    total+=timeRename();
  }
  total= total/20;
  printf("\n Average Time for Rename: %f seconds", total);*/
	printf("Average Time for Rename: %f seconds\n\n", time_rename());
	printf("Average Time for Link + Unlink: %f seconds\n\n", time_link_unlink());
	printf("Copying Throughput: %f bps\n\n", time_copy_file());
}
