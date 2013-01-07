#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include <git2.h>

#include "errors.h"
#include "git-support.h"
#include "repository.h"
#include "strbuf.h"
#include "utils.h"

static char *extract_option(const char *name, int *arg, int argc, char **argv)
{
	int name_len = strlen(name);
	char *argval = argv[*arg];

	if (prefixcmp(argval,name))
		return NULL;

	if (argval[name_len]=='=')
	{
		if (argval[name_len+1] == 0)
			return NULL;
		return &argval[name_len+1];
	}

	if (argval[name_len]!=0)
		return NULL;

	*arg = *arg + 1;
	if (*arg >= argc)
		return NULL;

	return argv[*arg];
}

int cmd_write_tree(git_repository *repo, int argc, char **argv)
{
	int i;
	int rc = EXIT_FAILURE;
	int err = 0;
	int verify_index = 1;

	int missing_ok = 0;
	char *prefix = NULL;

	char sha1buf[GIT_OID_HEXSZ + 1];

	for (i=1;i<argc;i++)
	{
		if (!strcmp("--missing-ok",argv[i])) missing_ok = 1;
		else
		{
			prefix = extract_option("--prefix",&i,argc,argv);
		}
	}

	verify_index = !missing_ok;

	git_index *idx;

	if ((err = git_repository_index(&idx, repo)) != GIT_OK)
		goto out;

	/* check the index */
	if (verify_index)
	{
		git_odb * odb;

		git_repository_odb(&odb, repo);

		for (size_t i = 0; i < git_index_entrycount(idx); i++)
		{
			const git_index_entry *gie = git_index_get_byindex(idx, i);

			if (git_odb_exists(odb, &gie->oid) != 1)
			{
				printf("error: invalid object %06o %s for '%s'\n", gie->mode, git_oid_tostr(sha1buf, GIT_OID_HEXSZ+1, &gie->oid), gie->path);
				printf("fatal: git-write-tree: error building trees\n");
				return EXIT_FAILURE;
			}
		}
	}

	git_oid oid;
	if ((err = git_index_write_tree_to(&oid, idx, repo)) != GIT_OK)
		goto out;

	printf("%s\n", git_oid_tostr(sha1buf, GIT_OID_HEXSZ+1, &oid));

	git_index_free(&idx);

	rc = EXIT_SUCCESS;
out:
	if(err != GIT_OK)
		libgit_error();

	return rc;
}

