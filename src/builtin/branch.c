#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <git2.h>
#include "checkout.h"

#include "cli/branch_cli.c"
#include "git-support.h"

int cmd_branch(git_repository *repo, int argc, char **argv)
{
	int err = GIT_OK;
	int rc = EXIT_FAILURE;

	int i;
	unsigned int list_flags;
	char *branch = NULL;

	git_branch_iterator *iter = NULL;
	git_reference *head_ref = NULL;
	git_reference *new_branch_ref = NULL;

	struct cli cli = {0};

	if (!parse_cli(argc - 1, &argv[1], &cli, POF_VALIDATE))
	{
		return GIT_ERROR;
	}

	if (usage_cli(argv[0], &cli))
	{
		return GIT_OK;
	}

	if ((cli.a || cli.r) && cli.pattern)
	{
		fprintf(stderr,"Options -a and -r do not make sense with a branch name");
		goto out;
	}

	list_flags = 0;

	if (cli.a)
	{
		list_flags = GIT_BRANCH_LOCAL | GIT_BRANCH_REMOTE;
	} else if (cli.r)
	{
		list_flags = GIT_BRANCH_REMOTE;
	}
	branch = cli.pattern;

	if (!branch)
	{
		struct git_reference *branch_ref;
		git_branch_t type;

		/* If no list flag has been specified, assume the default one */
		if (!list_flags) list_flags = GIT_BRANCH_LOCAL;

		if ((err = git_branch_iterator_new(&iter, repo, list_flags)))
			goto out;

		while (!git_branch_next(&branch_ref, &type, iter))
		{
			const char *branch_name;
			if (!git_branch_name(&branch_name, branch_ref))
				printf("%s%s\n",git_branch_is_head(branch_ref)?"* ":"  ",branch_name);
		}
	} else
	{
		if ((err = git_repository_head(&head_ref,repo)))
			goto out;

		if (cli->unset_upstream)
		{
			if ((err = git_branch_set_upstream(head_ref, NULL)))
				goto out;
		} else if (cli->set_upstream_to)
		{
			if ((err = git_branch_set_upstream(head_ref, cli->upstream)))
				goto out;
		} else
		{
			if ((err = git2_create_branch_from_ref(&new_branch_ref,head_ref,repo,branch)))
				goto out;
		}
	}

out:
	if (err != GIT_OK)
		libgit_error();

	if (iter) git_branch_iterator_free(iter);
	if (head_ref) git_reference_free(head_ref);
	if (new_branch_ref) git_reference_free(new_branch_ref);

	return rc;
}
