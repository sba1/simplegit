/**
 * @file
 */

#include "stash.h"

#include "errors.h"
#include "strbuf.h"

#include <git2.h>

static void print_usage(char *name)
{
	fprintf (stderr, "USAGE: %s [-d] [tagname]\n", name);
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
	int del = 0;

	if (argc == 3 && !strcmp("-d", argv[1]))
		del = 1;

	if (argc > 2 && !del)
	{
		print_usage(argv[0]);
		goto out;
	}
	if (argc == 1)
	{
		git_tag_foreach(repo, list_foreach_cb, NULL);
	} else
	{
		const char *tagname;
		git_reference *head_ref;
		git_object *head_obj;
		git_oid oid;

		tagname = del?argv[1]:argv[2];

		if ((err = git_repository_head(&head_ref,repo)))
			goto out;

		if ((err = git_reference_peel(&head_obj, head_ref, GIT_OBJ_COMMIT)))
			goto out;

		if (del)
		{
			if ((err = git_tag_delete(repo, tagname)))
				goto out;
		}	else
		{
			if ((err = git_tag_create_lightweight(&oid, repo, tagname, head_obj, 0)))
				goto out;
		}
	}

	rc = EXIT_SUCCESS;
out:
	if (err != GIT_OK)
		libgit_error();
	return rc;
}
