/**
 * @file
 */

#include "reset.h"

#include "errors.h"

#include <stdio.h>

#include <git2.h>

int cmd_reset(git_repository *repo, int argc, char **argv)
{
	int err = GIT_OK;
	int rc = EXIT_FAILURE;

	const char *spec;
	git_object *treeobj = NULL;

	int mode = -1;
	int spec_pos;

	if (argc < 3)
	{
		fprintf (stderr, "USAGE: %s <treeish> <paths>\n", argv[0]);
		fprintf (stderr, "USAGE: %s [--soft|--mixed|--hard] <commit>\n", argv[0]);
		return -1;
	}

	if (!strcmp("--soft", argv[1])) mode = GIT_RESET_SOFT;
	else if (!strcmp("--mixed", argv[1])) mode = GIT_RESET_MIXED;
	else if (!strcmp("--hard", argv[1])) mode = GIT_RESET_HARD;

	if (mode != -1) spec_pos = 2;
	else spec_pos = 1;

	spec = argv[spec_pos];

	if ((err = git_revparse_single(&treeobj, repo, spec)))
		goto out;

	if (mode == -1)
	{
		git_strarray strarray;

		strarray.count = argc-1-spec_pos;
		strarray.strings = argv+1+spec_pos;

		if ((err = git_reset_default(repo, treeobj, &strarray)))
			goto out;
	} else
	{
		if ((err = git_reset(repo, treeobj, mode, NULL)))
			goto out;
	}
out:
	if (treeobj) git_object_free(treeobj);

	if (err != GIT_OK)
		libgit_error();
	return rc;
}
