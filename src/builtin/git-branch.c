#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <git2.h>

#include "git-checkout.h"

struct branch_list_callback_payload
{
	git_repository *repo;
};

static int branch_list_callback(const char *branch_name, git_branch_t branch_type, void *payload)
{
	struct branch_list_callback_payload *pl = (struct branch_list_callback_payload*)payload;
	struct git_reference *branch_ref;

	if (git_branch_lookup(&branch_ref,pl->repo,branch_name,branch_type) != 0)
		branch_ref = NULL;

	printf("%s%s\n",git_branch_is_head(branch_ref)?"* ":"  ",branch_name);

	git_reference_free(branch_ref);
	return 0;
}

int cmd_branch(git_repository *repo, int argc, char **argv)
{
	int i;
	unsigned int list_flags;
	struct branch_list_callback_payload pl;

	list_flags = GIT_BRANCH_LOCAL;

	for (i=1;i<argc;i++)
	{
		if (!strcmp(argv[i],"-a")) list_flags = GIT_BRANCH_LOCAL | GIT_BRANCH_REMOTE;
		else if (!strcmp(argv[i],"-r")) list_flags = GIT_BRANCH_REMOTE;
	}

	pl.repo = repo;

	git_branch_foreach(repo,list_flags,branch_list_callback,&pl);

	return EXIT_SUCCESS;
}
