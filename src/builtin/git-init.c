#include "git-init.h"
#include "git-support.h"

int cmd_init(int argc, const char **argv){
	//TO-DO !
	(void)argc;
	(void)argv;

	please_git_do_it_for_me();
}

/*#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <git2.h>
#include "errors.h"


// Create an empty git repository or reinitialize an existing one
int main(int argc, char *argv[]) {
	if(argc != 2) {
		printf("Path unspecified !\n");
		return EXIT_FAILURE;
	}
	
	git_repository **repo_out;
	//A voir : comment faire le test entre si on met true ou false
	if (git_repository_init (repo_out, argv[argc-1], true) == GIT_ERROR) {
		printf("Error : Can't create the repository");
		break;
	}
	
	return EXIT_SUCCESS;
}*/
