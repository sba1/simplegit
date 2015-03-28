#include "rm.h"

#include <stdio.h>
#include <stdlib.h>

#include <git2.h> // IWYU pragma: keep

#include "errors.h"

static int cmd_rm_matched_paths_callback(const char *path, const char *matched_pathspec, void *payload)
{
	int *num_added = (int*)payload;
	*num_added = *num_added + 1;
	return 0;
}

int cmd_rm(git_repository *repo, int argc, char **argv)
{
	int err = GIT_OK;
	int rc = EXIT_FAILURE;
	git_index *idx = NULL;
	int num_added = 0;
	git_strarray paths;

	if (argc < 2)
	{
		fprintf (stderr, "USAGE: %s <paths>\n", argv[0]);
		return -1;
	}

	if ((err = git_repository_index(&idx,repo)) != GIT_OK)
		goto out;

	paths.count = argc - 1;
	paths.strings = &argv[1];

	if ((err = git_index_remove_all(idx, &paths, cmd_rm_matched_paths_callback, &num_added)))
		goto out;

	if (num_added)
	{
		if ((err = git_index_write(idx)) != GIT_OK)
			goto out;
	}

	rc = EXIT_SUCCESS;
out:
	if (idx) git_index_free(idx);
	if (err != GIT_OK)
		libgit_error();
	return rc;
}
