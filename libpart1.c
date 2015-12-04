#include <stdio.h>
#include <stdlib.h>
#include "support.h"
#include <string.h>

// hello() - print some output so we know the shared object loaded.
//
// The function signature takes an untyped parameter, and returns an untyped
// parameter.  In this way, the signature *could* support any behavior, by
// passing in a struct and returning a struct.
void *hello(void *input) {
    printf("hello from a .so\n");
    return NULL;
}

void toUpper(char * str) {
	int i = 0;
	char c;
	while(str[i]) {
		str[i] = toupper(str[i]);
		i++;
	}
}

void * ucase (void * input) {
	//printf("%s\n", (char *) input);
	struct team_t * team_tmp = (struct team_t *) input;
	struct team_t * team_cpy = (struct team_t *) malloc(sizeof(struct team_t));
/*
	team_cpy->name1 = (char *) malloc(strlen(team_tmp->name1) * sizeof(char));
	team_cpy->email1 = (char *) malloc(strlen(team_tmp->email1) * sizeof(char));
	team_cpy->name2 = (char *) malloc(strlen(team_tmp->name2) * sizeof(char));
	team_cpy->email2 = (char *) malloc(strlen(team_tmp->email2) * sizeof(char));
	
	strcpy(team_cpy->name1, team_tmp->name1);	
	strcpy(team_cpy->email1, team_tmp->email1);
	strcpy(team_cpy->name2, team_tmp->name2);
	strcpy(team_cpy->email2, team_tmp->email2);
*/
	team_cpy->name1 = strdup(team_tmp->name1);	
	team_cpy->email1 = strdup(team_tmp->email1);	
	team_cpy->name2 = strdup(team_tmp->name2);	
	team_cpy->email2 = strdup(team_tmp->email2);	

	toUpper(team_cpy->name1);
	toUpper(team_cpy->email1);
	toUpper(team_cpy->name2);
	toUpper(team_cpy->email2);	

	return team_cpy;	
}
