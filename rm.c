#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

int moveFile(const char* file, struct stat* fileStat);
int main(int argc, const char* argv[]){
	int i = 1;
	int fileCounter = 0;
	int optionCounter = 0;
  const char** files = (const char**) malloc((argc-1)*sizeof(char*));
	const char** options = (const char**)malloc(3*sizeof(char*));
	struct stat** stats = (struct stat**)malloc((argc-1)*sizeof (struct stat*));
	//loop to seperate files from options
	for (i; i<argc; i++){
		int length = strlen(argv[i]);
		if(argv[i][0]== '-' && length==2){
			options[optionCounter]=  argv[i];
			optionCounter++;
		}else{
			struct stat* fileStat = (struct stat*) malloc(sizeof(struct stat));
			if (stat(argv[i], fileStat) == -1){
				printf("error! invalid filepath:  %s \n", argv[i]);
			}else{
				stats[fileCounter] = fileStat;
				files[fileCounter] =  argv[i];
				printf("%ld \n",fileStat->st_size);
				fileCounter++;
			}
		}
	}
	//TODO handle optional CMD Line options
	for (i =0; i< fileCounter; i++){
		moveFile(files[i], stats[i]);
	}
}
	/**
Properties of stat
	st_mode 	The current permissions on the file.
	st_ino 		The inode for the file (note that this number is unique to all files and directories on a Linux System.
	st_dev 		The device that the file currently resides on.
	st_uid 		The User ID for the file.
	st_gid 		The Group ID for the file.
	st_atime 	The most recent time that the file was accessed.
	st_ctime 	The most recent time that the file's permissions were changed.
	st_mtime 	The most recent time that the file's contents were modified.
	st_nlink 	The number of links that there are to this file.
	st_size 	The size in bytes
*/

//Moves file to dumpster
int moveFile(const char* file, struct stat* fileStat){
	const char[] dumpsterLocation = "dumpster/";//TODO replace with path provide by enviromental variable DUMPSTER
	strcat(dumpsterLocation,file);
	//TODO add check for existing files and add ".1", ".2"... or print error message if ".9" exists
	//TODO use Stat to set permissions, access time, and maybe modified time
	//TODO use st_dev in stat to determine if rename will work (must be the same as dumpsterLocation), if it wont work use copy and unlink
	rename(file,dumpsterLocation);
}
