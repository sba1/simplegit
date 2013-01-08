#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <git2.h>

#include "errors.h"
#include "git-diff-files.h"
#include "git-support.h"
#include "repository.h"
#include "quote.h"

static int file_cb(const git_diff_delta *delta, float progress, void *payload)
{
	printf("%s\n",delta->new_file.path);
	return 0;
}

int cmd_diff_files(git_repository *repo, int argc, char **argv)
{
	int i;
	int err = 0;
	int rc = EXIT_FAILURE;
	int filec = 0;
	char **filev = NULL;

	git_index *idx = NULL;
	git_diff_list *d = NULL;

	git_diff_options opts;

	for (i=1;i<argc;i++)
	{
/*		if (!strcmp("-r",argv[i])) opts.recurse = 1;
		else if (!strcmp("-t",argv[i])) opts.show_tree_entries = 1;
		else */if (argv[i][0] != '-')
		{
			filev = &argv[i];
			filec = argc - i;
			break;
		} else
		{
			fprintf(stderr,"Unknown option \"%s\"\n",argv[i]);
			goto out;
		}
	}

	if ((err = git_repository_index(&idx,repo)) != GIT_OK)
		goto out;

	memset(&opts,0,sizeof(opts));
	opts.version = GIT_DIFF_OPTIONS_VERSION;

	if ((err = git_diff_index_to_workdir(&d,repo,idx,&opts)) != GIT_OK)
		goto out;

	if ((err = git_diff_foreach(d,file_cb,NULL,NULL,&opts)) != GIT_OK)
		goto out;

	rc = EXIT_SUCCESS;
out:
	if (err)
	{
		printf("%d\n",err);
		libgit_error();
	}
	if (idx) git_index_free(idx);
	return rc;
}
