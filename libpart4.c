#include <stdint.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
/*
 * We aren't providing much code here.  You'll need to implement your own
 * dlsym(), as well as any constructors or destructors for your library.
 */

/* TODO: Your code goes here */

void parseInfo(char * info, char ** module)
{
    char * tok = (char*)strtok(info, "/");
    char * str;
    while(tok != NULL)
    {
        str = (char*)strdup(tok);
	tok = (char*)strtok(NULL, "/");
    }
    *module = (char*)strtok(str, "\t ");
}

__attribute__((constructor))
static void initialize_library()
{
}

__attribute__((destructor))
static void destroy_library()
{
}

typedef int (*socket_alias)(int, int, int);
static socket_alias std_socket = NULL;
int socket(int domain, int type, int protocol)
{
	if(std_socket == NULL)
		std_socket = dlsym(RTLD_NEXT, "socket");
	char * error = dlerror();
	if(error != NULL)
	{
	    fprintf(stderr, "Error finding original socket: %s\n", error);
	}
	int i, ptr_count;
        int back_size = 10;
	void * buffer[back_size];
        char ** trace_strings;
	ptr_count = (int)backtrace(buffer, back_size);
        trace_strings = (char**)backtrace_symbols(buffer, ptr_count);
    	if(trace_strings != NULL)
    	{
		char * info, * module;
	        int x;
		for(x = 0; x < ptr_count; x++)
		{
		    //printf with a ( first so we don't account for other print functions (fprintf, sprintf, etc)
		    if(strstr(trace_strings[x], "(printf") != NULL || strstr(trace_strings[x], "(scanf") != NULL)
		    {
		        //protect our code
			// we can only mprotect a page at a time. Therefore, we begin
			// mprotecting from the start of the page that contains the
			// address to our socket function, and continue until
			// we have made the function non-executable, which will throw 
			// a segmentation fault.
			uintptr_t socket_start = (uintptr_t) &socket;
			size_t page_size = sysconf(_SC_PAGESIZE);
			uintptr_t page_start = socket_start & -page_size;
			while(mprotect((void*)page_start, page_size, PROT_NONE) < 0)
			{
			    page_start += page_size;
			}
		        break;
		    }
		}
		free(trace_strings);
	}
	int ret_val = std_socket(domain, type, protocol);
	return ret_val;
}
