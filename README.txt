# CS4513Project1

Use the make command in this area to generate our rm dv and dump programs

"source set_dumpster.sh" will initialize the dumpster to a default dumpster/ in working directory 
	and will change the path to include all of our custom commands globally

Use rm to run our version of the rm command. By default it moves a file to a folder specified by the dumpster enviromental variable. If a file exists with that name in the dumpster it add an extension .1 - .9 as appropriate. If all the extensions are taken it throws an error
	-r Remove directories recursively
	-f Force removal without backing up to the dumpster
	-d Specify a dumpster directory
	-h Display this help and exit
 
Use dv to run our version of the dv (dive) command to retrieve a specified file from dumpster. If the specified does not exist it will print an error message.
	-d Specify a dumpster directory
	-h Display this help and exit

Use dump to run our version of the dump command. By deafault it removes all files in the folder specified by the enviromental variable.
	 -d Specify a dumpster directory
	 -h Display this help and exit

Use the "make exp" command to compile our experiment code. 

Use ./experiments to run our tests

Use make clean to remove all .o and executable files


dv.c, dump.c, rm.c and experiments.c contain the code for their respective commands

util.c contains utility functions such as try and fail that are used by all of our executables.

futil.c contains file utility functions necessary for all commands except experiments.c 
