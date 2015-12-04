#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "support.h"
/*
 * load_and_invoke() - load the given .so and execute the specified function
 */
void load_and_invoke(char *libname, char *funcname) {
    /* TODO: complete this function */
	if (!libname || !funcname) {
		fprintf(stderr, "part1b needs both a library name and a function name\n");
		exit(0);
	}	
	void * lib_handle = (void *) dlopen(libname, RTLD_NOW);
	if (lib_handle == NULL) {
	    fprintf(stderr, "invalid library name\n");
	    exit(0);
	}
	dlerror();

	typedef void * (* function_alias) (void *);
	function_alias func;
	func = (function_alias) dlsym(lib_handle, funcname);
	
	char * error_msg = (char *) dlerror();
	if (error_msg != NULL) {
	    fprintf(stderr, "invalid function name\n");
	    exit(0);
	}
	void * ret_val = (*func) NULL;
	dlclose(lib_handle);
}

/*
 * help() - Print a help message.
 */
void help(char *progname) {
    printf("Usage: %s [OPTIONS]\n", progname);
    printf("Load the given .so and run the requested function from that .so\n");
    printf("  -l [string] The name of the .so to load\n");
    printf("  -f [string] The name of the function within that .so to run\n");
}

/*
 * main() - The main routine parses arguments and invokes hello
 */
int main(int argc, char **argv) {
    /* for getopt */
    long opt;
    char * load;
    char * run;

    /* run a student name check */
    check_team(argv[0]);

    /* parse the command-line options.  For this program, we only support  */
    /* the parameterless 'h' option, for getting help on program usage. */
    while ((opt = getopt(argc, argv, "hl:f:")) != -1) {
        switch(opt) {
          case 'h': help(argv[0]); break;
          case 'l': load = optarg; break;
	  case 'f': run = optarg; break;
	}
    }
    
	/* call load_and_invoke() to run the given function of the given library */
    load_and_invoke(load, run);
    exit(0);
}
