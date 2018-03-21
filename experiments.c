/**
 * tests.c
 * Test for the program
 *
 * @author 	Drew Ciccarelli
 * @since	3/12/2018
 * @version 1.0
 *
 */
 #include "util.h"
 #include <sys/time.h>
#define BLOCK_SIZE 	4096
 #define TimeVal struct timeval


/**
Times renaming a test file
*/
double timeRename(){
  TimeVal* startTime= (TimeVal*) malloc(sizeof(TimeVal));
  TimeVal* endTime= (TimeVal*) malloc(sizeof(TimeVal));
  open("test1", O_CREAT);
  sync();
  gettimeofday(startTime, NULL);
  rename("test1", "dumpster/test1");
  sync();
  gettimeofday(endTime, NULL);
  double diff = ((double)endTime->tv_sec +((double)endTime->tv_usec)/1000000)- ((double)startTime->tv_sec+ ((double)startTime->tv_usec)/1000000);
  //printf("%f",diff );
  return diff;
}


/**
 * Times the Copying of the contents of a file into the file with the given name
 *
 * @param from	The original file to copy
 * @param to	The name of the file to copy to
 *
 * @return The bps for the file copy
 */
int time_copy_file(const char* from, const char* to) {
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
/**
*/
int main(){
  double total = 0;
  for (int i =0; i < 20; i++){
    total+=timeRename();
    
  }
  total= total/20;
  printf("\n Average Time for Rename: %f seconds", total);


}
