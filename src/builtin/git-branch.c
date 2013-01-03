#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <git2.h>

#include "git-checkout.h"
#include "git-support.h"

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
	char *branch = NULL;

	list_flags = 0;

	for (i=1;i<argc;i++)
	{
		if (argv[i][0] != '-')
		{
			branch = argv[i];
		}
		else if (!strcmp(argv[i],"-a")) list_flags = GIT_BRANCH_LOCAL | GIT_BRANCH_REMOTE;
		else if (!strcmp(argv[i],"-r")) list_flags = GIT_BRANCH_REMOTE;
	}

	if (branch && list_flags)
	{
		fprintf(stderr,"Options -a and -r do not make sense with a branch name");
		return EXIT_FAILURE;

	}

	if (!branch)
	{
		struct branch_list_callback_payload pl;
		pl.repo = repo;
		git_branch_foreach(repo,list_flags,branch_list_callback,&pl);
	} else
	{
		int err;

		git_reference *head_ref;

		if ((err = git_repository_head(&head_ref,repo)) == 0)
		{
			git_reference *new_branch_ref;
			if ((err = git2_create_branch_from_ref(&new_branch_ref,head_ref,repo,branch)) == 0)
			{
				git_reference_free(new_branch_ref);
			}
			git_reference_free(head_ref);
		}
	}

	return EXIT_SUCCESS;
}
