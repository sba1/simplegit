#include "status.h"

#include <stdlib.h>
#include <stdio.h>

#include <git2.h>

#include "errors.h"
#include "git-support.h"
#include "utils.h"

#define GIT_STATUS_WT (GIT_STATUS_WT_MODIFIED|GIT_STATUS_WT_NEW|GIT_STATUS_WT_TYPECHANGE|GIT_STATUS_WT_DELETED)

/**
 * The function that is invoked for each status entry. It prints out the
 * status to the standard output stream.
 *
 * @param path the path to the file for which this status is reported
 * @param status_flags the status of the file
 * @param payload an integer that defines which kind of path entries are
 *  interesting. It also holds holds the header print state.
 * @return
 */
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

	if (((*mode) & 1) == 0)
	{
		switch (*mode >> 1)
		{
		case	0:
				printf("# Changes to be committed:\n#\n");
				break;

		case	1:
				printf("#\n# Changes not staged for commit:\n#\n");
				break;

		case	2:
				printf("#\n# Untracked files:\n#\n");
				break;
		}
		*mode |= 1;
	}
	if (*mode >> 1 == 2)
	{
		if (status_flags & GIT_STATUS_WT_NEW)
			printf("#       %s\n",path);
	} else
	{
		if (!(status_flags & GIT_STATUS_WT_NEW))
			printf("#       %s: %s\n",txt,path);
	}

	return 0;
}

int cmd_status(git_repository *repo, int argc, char **argv)
{
	int rc = EXIT_FAILURE;
	int err = 0;
	int i, mode;

	git_reference *head_ref = NULL;
	git_reference *upstream_ref = NULL;

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
		{
			int num_parents = 0;
			sgit_repository_mergeheads_count(&num_parents, repo);
			if (num_parents)
			{
				printf("# On branch %s, but merging\n",branch_name);
			} else
			{
				printf("# On branch %s\n",branch_name);
			}
		}
	} else
	{
		printf("# On an unnamed branch\n");
	}

	if (!(err = git_branch_upstream(&upstream_ref, head_ref)))
	{
		const git_oid *local, *upstream;

		local = git_reference_target(head_ref);
		upstream = git_reference_target(upstream_ref);

		if (local && upstream)
		{
			const char *upstream_name;
			size_t ahead, behind;

			if ((err = git_branch_name(&upstream_name, upstream_ref)))
				goto out;

			if ((err = git_graph_ahead_behind(&ahead, &behind, repo, local, upstream)))
				goto out;

			if (ahead && behind)
			{
				printf("# Your branch and its upstream branch '%s' have diverged.\n", upstream_name);
				printf("# They have %d and %d different commits each, respectively.\n", (int)ahead, (int)behind);
			} else if (behind)
			{
				printf("# Your branch is behind its upstream branch '%s' by %d commits.\n", upstream_name, (int)behind);
			} else if (ahead)
			{
				printf("# Your branch is ahead its upstream branch '%s' by %d commits.\n", upstream_name, (int)ahead);
			}
		}
	} else if (err != GIT_ENOTFOUND)
	{
		goto out;
	}

	/* FIXME: Use git_status_list() API */
	opt.flags = GIT_STATUS_OPT_INCLUDE_UNMODIFIED |
			GIT_STATUS_OPT_INCLUDE_UNTRACKED |
			GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS;
	opt.show = GIT_STATUS_SHOW_INDEX_ONLY;

	/* Index modifications */
	mode = 0<<1;
	if ((err = git_status_foreach_ext(repo,&opt,status_cb,&mode)) != GIT_OK)
		goto out;

	mode = 1<<1;

	/* Workdir vs Index modifications */
	opt.show = GIT_STATUS_SHOW_WORKDIR_ONLY;
	if ((err = git_status_foreach_ext(repo,&opt,status_cb,&mode)) != GIT_OK)
		goto out;

	/* Dito, but only new files are printed */
	mode = 2<<1;
	if ((err = git_status_foreach_ext(repo,&opt,status_cb,&mode)) != GIT_OK)
		goto out;

	printf("#\n");
	rc = EXIT_SUCCESS;
out:
	if (err) libgit_error();
	if (head_ref) git_reference_free(head_ref);
	return rc;
}
