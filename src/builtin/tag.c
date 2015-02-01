/**
 * @file
 */

#include "stash.h"

#include "errors.h"
#include "strbuf.h"

#include <git2.h>

static void print_usage(char *name)
{
	fprintf (stderr, "USAGE: %s\n", name);
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

	if (argc != 1)
	{
		print_usage(argv[0]);
		goto out;
	}
	git_tag_foreach(repo, list_foreach_cb, NULL);
	rc = EXIT_SUCCESS;
out:
	if (err != GIT_OK)
		libgit_error();
	return rc;
}
