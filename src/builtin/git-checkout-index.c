#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <git2.h>

#include "errors.h"
#include "git-checkout-index.h"

static int notify_cb(git_checkout_notify_t why,
	const char *path,
	const git_diff_file *baseline,
	const git_diff_file *target,
	const git_diff_file *workdir,
	void *payload)
{
	fprintf(stderr,"Notify in %s\n",path);
	baseline = baseline;
	target = target;
	workdir = workdir;
	payload = payload;
	why = why;
	return 1;
}

int cmd_checkout_index(git_repository *repo, int argc, char **argv)
{
	int i, err, rc;
	int all = 0;
	git_index *idx = NULL;
	git_checkout_opts checkout_opts;

	err = 0;
	rc = EXIT_FAILURE;

	for (i=1;i<argc;i++)
	{
		if (argv[i][0] != '-')
		{
			fprintf(stderr,"File arguments not supported!\n");
			goto out;
		} else
		{
			if (!strcmp("--all",argv[i])) all = 1;
			else
			{
				fprintf(stderr,"Unknown option \"%s\"\n",argv[i]);
				goto out;
			}
		}
	}
	
	if (!all)
	{
		fprintf(stderr,"Only --all is supported for now!\n");
		goto out;
	}
	if ((err = git_repository_index(&idx,repo)) != GIT_OK)
		goto out;

	/* Default options. Note by default we perform a dry checkout */
	memset(&checkout_opts,0,sizeof(checkout_opts));
	checkout_opts.version = GIT_CHECKOUT_OPTS_VERSION;
	checkout_opts.notify_cb = notify_cb;

	checkout_opts.checkout_strategy = GIT_CHECKOUT_FORCE|GIT_CHECKOUT_REMOVE_UNTRACKED;//GIT_CHECKOUT_SAFE|GIT_CHECKOUT_UPDATE_UNTRACKED;
	if (git_checkout_index(repo,idx,&checkout_opts) != 0)
	{
		libgit_error();
		goto out;
	}

	rc = EXIT_SUCCESS;
out:
	if (idx) git_index_free(idx);
	if (err) libgit_error();
	return rc;
}
