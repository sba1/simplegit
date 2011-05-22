#include "builtin.h"
#include "exec-cmd.h"
#include "git-support.h"

#include <string.h>

// original source : https://github.com/vfr-nl/git2

#include <stdio.h>

cmd_handler lookup_handler(char *cmd){
	unsigned int i;
	unsigned int max = sizeof(commands) / sizeof(cmd_struct);

	for (i = 0; i < max; i++)
		if (!strcmp(commands[i].cmd, cmd))
			return commands[i].handler;
	return NULL;
}

int main(int argc, char **argv){
	git_support_register_arguments(argc, argv);

	//register argument so that we can fallback to git
	//if we can't achieve the job
	
	cmd_handler handler;

	if (argc < 2) {
		please_git_do_it_for_me();
	}

	if ((handler = lookup_handler(argv[1])) == NULL) {
		please_git_do_it_for_me();
	}

	return handler(argc-2, (const char**)(argv+2));
}
