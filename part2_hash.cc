#include <unordered_map>
#include <cassert>
#include <cstdio>

using namespace std;

/*
 * Note: 'uintptr_t' is a special type of unsigned integer that is guaranteed
 *       to be the same size as pointers.  This is the preferred way to cast
 *       between pointers and integers in code that could be compiled in
 *       32-bit or 64-bit mode.
 */

/*
 * A map from pointers to strings.  When an allocation is made, this will
 * store the value returned by the alloc, and the string corresponding to the
 * module from which the allocation was made.
 */
unordered_map<uintptr_t, char*> * alloc_modules = NULL;

/*
 * A map from pointers to strings.  When an allocation is made, this will
 * store the value returned by the alloc, and the string corresponding to the
 * program counter at the time the allocation was made.
 */
unordered_map<uintptr_t, char*> * alloc_progcounters = NULL;

/*
 * declare the following functions to have "C" linkage, so that we can see
 * them from C code without doing name demangling.
 */
extern "C"
{
    void map_initialize()
    {
        if(alloc_modules == NULL)
	{
	    alloc_modules = new unordered_map<uintptr_t, char*>();
	}
	if(alloc_progcounters == NULL)
	{
	    alloc_progcounters = new unordered_map<uintptr_t, char*>();
	}
    }
    void map_delete()
    {
        if(alloc_modules != NULL)
	{
	    delete alloc_modules;
	    alloc_modules = NULL;
	}
	if(alloc_progcounters != NULL) 
	{
	    delete alloc_progcounters;
            alloc_modules = NULL;
	}
    }
    /*
     * insert() - when malloc() is called, your interpositioning library
     *            should use this to store the pointer that malloc returned,
     *            along with the module name and line of code that
     *            backtrace() returned.  Return 1 if the pointer was
     *            successfully added, and 0 if the pointer was already in the
     *            map.
     */
    int map_insert(uintptr_t pointer, char *module, char *line) {
        /* TODO: complete this code */
        map_initialize();
	auto it = alloc_modules->find(pointer);
	if(it != alloc_modules->end())
	    return 0;
	else
	{
	    alloc_modules->insert(std::make_pair(pointer, module));
	    alloc_progcounters->insert(std::make_pair(pointer, line));
	    return 1;
	}
	
    }

    /*
     * remove() - when free() is called, your interpositioning library should
     *            use this to remove the pointer and its strings from the two
     *            maps declared above.  In this way, you'll be able to track,
     *            at all times, which allocations are outstanding, and what
     *            line of code caused those allocations.  Return 1 if the
     *            remove was successful, and 0 if the pointer was already
     *            removed from the map (which would suggest a double-free).
     */
    int map_remove(uintptr_t pointer) {
        /* TODO: complete this code */
	map_initialize();
	auto it = alloc_modules->find(pointer);
	if(it == alloc_modules->end())
	    return 0;
	else
	{
	    alloc_modules->erase(pointer);
	    alloc_progcounters->erase(pointer);
	    return 1;
	}
    }

    /*
     * count() - return the number of elements in the map.  This can indicate
     *           that there are un-freed allocations (memory leaks).
     */
    int map_count() {
        assert(alloc_modules->size() == alloc_progcounters->size());
        return alloc_modules->size();
    }

    /*
     * dump() - output the contents of the maps
     */
    void map_dump() {
        for (auto& key : *alloc_modules) {
            char *pc = alloc_progcounters->find(key.first)->second;
            printf("  0x%x allocated by %s::%s\n", key.first, key.second, pc);
        }
    }
}
