#include <stdint.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*
 * We aren't providing much code here.  You'll need to implement your own
 * malloc() and free(), as well as any constructors or destructors for your
 * library.
 */

/*
 * Declarations for the functions in part2_hash.cc, so that we don't need an
 * extra header file.
 */
void map_initialize();
void map_delete();
int map_insert(uintptr_t pointer, char *module, char *line);
int map_remove(uintptr_t pointer);
int map_count();
void map_dump();

/* TODO: Your code goes here */

void parseInfo(char * info, char ** module, char ** line)
{
    char * tok = (char*)strtok(info, "/");
    char * str;
    while(tok != NULL)
    {
        str = (char*)strdup(tok);
	tok = (char*)strtok(NULL, "/");
    }
    *module = (char*)strtok(str, "\t ");
    *line = (char*)strtok(NULL, "");
    (*line)++;
    (*line)[strlen(*line) - 1] = '\0';
}

__attribute__((constructor))
static void initialize_library()
{
    map_initialize();
}
__attribute__((destructor))
static void destroy_library()
{
    int reachable_blocks = map_count();
    printf("%d blocks still reachable\n", reachable_blocks);
    if(reachable_blocks > 0)
    {
        map_dump();
    }
    map_delete();
}

static int static_flag = 0;
typedef void * (*malloc_alias)(size_t);
static malloc_alias std_malloc = NULL;
void * malloc(size_t size)
{
    if(std_malloc == NULL)
        std_malloc = dlsym(RTLD_NEXT, "malloc");
    void * pointer = std_malloc(size);
    if(static_flag)
    {
        return pointer;
    }
    static_flag = 1;
    int i, ptr_count;
    void * buffer[10];
    char ** trace_strings;
    ptr_count = (int)backtrace(buffer, 10);
    trace_strings = (char**)backtrace_symbols(buffer, ptr_count);
    if(trace_strings != NULL)
    {
        char * info, * module, * line;
	info = trace_strings[1];
	parseInfo(info, &module, &line);
	map_insert((uintptr_t)pointer, module, line);
        free(trace_strings);
    }
    static_flag = 0;
    return pointer;
}

typedef void (*free_alias)(void *);
static free_alias std_free = NULL;
void free(void * pointer)
{
    if(std_free == NULL)
        std_free = dlsym(RTLD_NEXT, "free");
    map_remove((uintptr_t)pointer);
    std_free(pointer);
}
