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
	git_branch_foreach(repo,GIT_BRANCH_LOCAL,branch_list_callback,NULL);
	return EXIT_SUCCESS;
}
