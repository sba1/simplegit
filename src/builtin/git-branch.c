#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <git2.h>

#include "git-checkout.h"

static int branch_list_callback(const char *branch_name, git_branch_t branch_type, void *payload)
{
	printf("%s\n",branch_name);
	return 0;
}

int cmd_branch(git_repository *repo, int argc, char **argv)
{
	int i;
	unsigned int list_flags = GIT_BRANCH_LOCAL;

	for (i=1;i<argc;i++)
	{
		if (!strcmp(argv[i],"-a")) list_flags = GIT_BRANCH_LOCAL | GIT_BRANCH_REMOTE;
		else if (!strcmp(argv[i],"-r")) list_flags = GIT_BRANCH_REMOTE;
	}

	git_branch_foreach(repo,list_flags,branch_list_callback,NULL);
	return EXIT_SUCCESS;
}
