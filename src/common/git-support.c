#include "git-support.h"
#include "utils.h"
#include "errors.h"
#include <unistd.h>

char *please_git_help_me(int argc, char **argv){
	//TO-DO !
	
	(void)argc;

	return argv[0];
}

char *please_git_help_me2(char *command_line){
	//TO-DO !
	
	return command_line;
}

static char **git_argv;

void please_git_do_it_for_me(){
	execvp(git_argv[0], git_argv);
	die_errno("Failed to fallback to git.");
}

void git_support_register_arguments(int argc, char **argv){
	git_argv = (char**)xmalloc(sizeof(char*) * (argc + 1));

	git_argv[0] = (char*)xstrdup("git");

	for (int i = 1; i < argc; ++i) {
		git_argv[i] = (char*)xstrdup(argv[i]);
	}

	git_argv[argc] = NULL;
}
