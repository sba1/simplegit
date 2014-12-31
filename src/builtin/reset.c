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
	git_reference *head_ref = NULL;
	git_object *head = NULL;

	if (argc < 3)
	{
		fprintf (stderr, "USAGE: %s <treeish> <paths>\n", argv[0]);
		return -1;
	}

	if (strcmp(argv[1],"HEAD"))
	{
		fprintf(stderr, "Only HEAD is supported as first argument for now!\n");
		return -1;
	}

	strarray.count = argc-2;
	strarray.strings = argv+2;

	if ((err = git_repository_head(&head_ref,repo)))
		goto out;

	if ((err = git_reference_peel(&head, head_ref, GIT_OBJ_COMMIT)))
		goto out;

	if ((err = git_reset_default(repo, head, &strarray)))
		goto out;
out:
	if (head) git_object_free(head);
	if (head_ref) git_reference_free(head_ref);

	if (err != GIT_OK)
		libgit_error();
	return rc;
}
