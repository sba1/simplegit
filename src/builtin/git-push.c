#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <git2.h>

#include "errors.h"
#include "git-push.h"

static int push_status_callback(const char *ref, const char *msg, void *data)
{
	printf("%p\n",msg);
	return 0;
}

int cmd_push(git_repository *repo, int argc, char **argv)
{
	int err = GIT_OK;
	int i;
	int rc = EXIT_FAILURE;

	git_push *p = NULL;
	git_remote *r = NULL;

	for (i=1;i<argc;i++)
	{
		if (argv[i][0] == '-')
		{
			fprintf(stderr,"Unknown option \"%s\"\n",argv[i]);
			goto out;
		}

		if (r)
		{
			fprintf(stderr,"Only one argument supported for now!\n");
			goto out;
		}
		if ((err = git_remote_load(&r,repo,argv[i])) != GIT_OK)
			goto out;
	}

	if (!r)
	{
		fprintf(stderr,"No remote given!\n");
		goto out;
	}

	if ((err = git_push_new(&p,r)) != GIT_OK)
		goto out;

	if ((err = git_push_status_foreach(p,push_status_callback,NULL)) != GIT_OK)
		goto out;

	if ((err = git_push_add_refspec(p,"refs/heads/master")) != GIT_OK)
		goto out;

	if ((err = git_push_finish(p)) != GIT_OK)
		goto out;

	printf("done\n");
out:
	if (err != GIT_OK)
		libgit_error();
	if (p) git_push_free(p);
	if (r) git_remote_free(r);
	return rc;
}
