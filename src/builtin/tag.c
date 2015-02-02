/**
 * @file
 */

#include "stash.h"

#include "errors.h"
#include "strbuf.h"

#include <git2.h>

static void print_usage(char *name)
{
	fprintf (stderr, "USAGE: %s [tagname]\n", name);
}

static int list_foreach_cb(const char *name, git_oid *oid, void *payload)
{
	const char *refs_tags = "refs/tags/";
	if (!prefixcmp(name, refs_tags))
		printf("%s\n", &name[strlen(refs_tags)]);
	else
		printf("%s\n", name);
	return 0;
}

int cmd_tag(git_repository *repo, int argc, char **argv)
{
	int err = GIT_OK;
	int rc = EXIT_FAILURE;

	if (argc > 2)
	{
		print_usage(argv[0]);
		goto out;
	}
	if (argc == 1)
	{
		git_tag_foreach(repo, list_foreach_cb, NULL);
	} else
	{
		git_reference *head_ref;
		git_object *head_obj;
		git_oid oid;

		if ((err = git_repository_head(&head_ref,repo)))
			goto out;

		if ((err = git_reference_peel(&head_obj, head_ref, GIT_OBJ_COMMIT)))
			goto out;

		if ((err = git_tag_create_lightweight(&oid, repo, argv[1], head_obj, 0)))
			goto out;
	}

	rc = EXIT_SUCCESS;
out:
	if (err != GIT_OK)
		libgit_error();
	return rc;
}
