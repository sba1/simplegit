/**
 * @file
 */

#include "reset.h"

#include "errors.h"

#include <git2.h>

int cmd_reset(git_repository *repo, int argc, char **argv)
{
	int err = GIT_OK;
	int rc = EXIT_FAILURE;

	git_strarray strarray;
	const char *spec;
	git_object *treeobj = NULL;

	if (argc < 3)
	{
		fprintf (stderr, "USAGE: %s <treeish> <paths>\n", argv[0]);
		return -1;
	}

	spec = argv[1];

	strarray.count = argc-2;
	strarray.strings = argv+2;

	if ((err = git_revparse_single(&treeobj, repo, spec)))
		goto out;

	if ((err = git_reset_default(repo, treeobj, &strarray)))
		goto out;
out:
	if (treeobj) git_object_free(treeobj);

	if (err != GIT_OK)
		libgit_error();
	return rc;
}
