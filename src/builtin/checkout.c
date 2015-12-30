#include "checkout.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <git2.h>
#include "errors.h"
#include "git-support.h"

static int notify_cb(git_checkout_notify_t why,
	const char *path,
	const git_diff_file *baseline,
	const git_diff_file *target,
	const git_diff_file *workdir,
	void *payload)
{
	fprintf(stderr,"Notify in %s\n",path);
	return 1;
}

int cmd_checkout(git_repository *repo, int argc, char **argv)
{
	int i, err, rc;
	char *branch;
	git_reference *branch_ref;
	git_checkout_options checkout_opts;

	branch = NULL;
	rc = EXIT_FAILURE;

	for (i=1;i<argc;i++)
	{
		if (argv[i][0] != '-')
		{
			if (!branch) branch = argv[i];
		} else
		{
		}
	}

	if (!branch)
	{
		fprintf (stderr, "USAGE: %s <branch>\n", argv[0]);
		return -1;
	}

	/* Try local branch */
	if (git_branch_lookup(&branch_ref,repo,branch,GIT_BRANCH_LOCAL) != 0)
	{
		/* No, try remote branch */
		git_reference *remote_branch_ref;
		char remote_buf[256];
		snprintf(remote_buf,sizeof(remote_buf),"%s/%s","origin",branch);
		if (git_branch_lookup(&remote_branch_ref, repo, remote_buf, GIT_BRANCH_REMOTE) == 0)
		{
			/* Exists, now try to create a local one from that reference */
			if ((err = git2_create_branch_from_ref(&branch_ref,remote_branch_ref,repo,branch)) != 0)
			{
				fprintf(stderr,"Error code %d\n",err);
				libgit_error();
				goto out;
			}
			git_reference_free(remote_branch_ref);
		} else
		{
			branch_ref = NULL;
		}
	}

	printf("Checking out %s\n",branch_ref?git_reference_name(branch_ref):branch);
	if ((err = git_repository_set_head(repo,branch_ref?git_reference_name(branch_ref):branch)) != 0)
	{
		fprintf(stderr,"Error code %d\n",err);
		libgit_error();
		goto out;
	}

	/* Default options. Note by default we perform a dry checkout */
	memset(&checkout_opts,0,sizeof(checkout_opts));
	checkout_opts.version = GIT_CHECKOUT_OPTIONS_VERSION;
	checkout_opts.notify_cb = notify_cb;

	checkout_opts.checkout_strategy = GIT_CHECKOUT_FORCE|GIT_CHECKOUT_REMOVE_UNTRACKED;//GIT_CHECKOUT_SAFE|GIT_CHECKOUT_UPDATE_UNTRACKED;
	if (git_checkout_head(repo,&checkout_opts) != 0)
	{
		libgit_error();
		goto out;
	}

	rc = EXIT_SUCCESS;
out:
	if (branch_ref)
		git_reference_free(branch_ref);

	return rc;
}
