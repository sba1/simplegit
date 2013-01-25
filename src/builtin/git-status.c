#include <stdlib.h>
#include <stdio.h>

#include <git2.h>

#include "errors.h"
#include "git-status.h"
#include "git-support.h"
#include "repository.h"
#include "utils.h"

#define GIT_STATUS_WT (GIT_STATUS_WT_MODIFIED|GIT_STATUS_WT_NEW|GIT_STATUS_WT_TYPECHANGE|GIT_STATUS_WT_DELETED)

static int status_cb(const char *path, unsigned int status_flags, void *payload)
{
	char *txt;
	int *mode = (int*)payload;

	if (!status_flags)
		return 0;

	txt = "unknown";

	if (status_flags & GIT_STATUS_INDEX_NEW) txt = "new file";
	else if (status_flags & GIT_STATUS_INDEX_MODIFIED) txt = "modified";
	else if (status_flags & GIT_STATUS_WT_MODIFIED) txt = "modified";
	else if (status_flags & GIT_STATUS_WT_NEW) txt = "new file";
	else if (status_flags & GIT_STATUS_WT_TYPECHANGE) txt = "typechange";
	else if (status_flags & GIT_STATUS_WT_DELETED) txt = "deleted";

	if (*mode == 0 && (status_flags & GIT_STATUS_WT))
	{
		*mode = 1;
		printf("#\n# Changes not staged for commit:\n#\n");
	}
	printf("#       %s: %s (%d)\n",txt,path,status_flags);

	return 0;
}

int cmd_status(git_repository *repo, int argc, char **argv)
{
	int rc = EXIT_FAILURE;
	int err = 0;
	int i;
	int mode = 0;

	git_reference *head_ref = NULL;

	git_status_options opt = GIT_STATUS_OPTIONS_INIT;

	for (i=1;i<argc;i++)
	{
		if (argv[i][0] != '-')
		{
			fprintf(stderr,"No args supported at the moment\n");
			goto out;
		} else
		{
			fprintf(stderr,"Unknown option \"%s\"\n",argv[i]);
			goto out;
		}
	}

	if ((git_repository_head(&head_ref,repo)) == GIT_OK)
	{
		const char *branch_name;

		if ((err = git_branch_name(&branch_name,head_ref)) == GIT_OK)
			printf("# On branch %s\n",branch_name);
	} else
	{
		printf("# On an unnamed branch\n");
	}

	printf("# Changes to be committed:\n#\n");
	opt.flags = GIT_STATUS_OPT_INCLUDE_UNMODIFIED |
			GIT_STATUS_OPT_INCLUDE_UNTRACKED |
			GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS;
	opt.show = GIT_STATUS_SHOW_INDEX_THEN_WORKDIR;

	if ((err = git_status_foreach_ext(repo,&opt,status_cb,&mode)) != GIT_OK)
		goto out;

	rc = EXIT_SUCCESS;
out:
	if (err) libgit_error();
	if (head_ref) git_reference_free(head_ref);
	return rc;
}
