#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include <git2.h>

#include "errors.h"
#include "git-support.h"
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

	git_index *idx = NULL;
	git_index *repo_idx = NULL;

	char sha1buf[GIT_OID_HEXSZ + 1];

	for (i=1;i<argc;i++)
	{
		if (!strcmp("--missing-ok",argv[i])) missing_ok = 1;
		else
		{
			prefix = extract_option("--prefix",&i,argc,argv);
		}
	}

	if (!prefix) prefix = "";

	verify_index = !missing_ok;

	if ((err = git_repository_index(&repo_idx, repo)) != GIT_OK)
		goto out;

	if (*prefix)
	{
		if ((err = git_index_new(&idx)) != GIT_OK)
			goto out;

		/* Transfer entries to the in-memory index, omitting all that
		 * don't start with the given prefix */
		for (size_t i = 0; i < git_index_entrycount(repo_idx); i++)
		{
			const git_index_entry *gie = git_index_get_byindex(repo_idx, i);
			if (!prefixcmp(gie->path,prefix))
			{
				git_index_entry ngie = *gie;

				/* Note that the paths are rerooted */
				ngie.path = gie->path + strlen(prefix);
				if ((err = git_index_add(idx,&ngie)))
					goto out;
			}
		}
	} else
	{
		/* Take the repo index as in-memory index */
		idx = repo_idx;
		repo_idx = NULL;
	}

	/* check the index */
	if (verify_index)
	{
		git_odb * odb;

		git_repository_odb(&odb, repo);

		for (size_t i = 0; i < git_index_entrycount(idx); i++)
		{
			const git_index_entry *gie = git_index_get_byindex(idx, i);

			if (git_odb_exists(odb, &gie->id) != 1)
			{
				printf("error: invalid object %06o %s for '%s'\n", gie->mode, git_oid_tostr(sha1buf, GIT_OID_HEXSZ+1, &gie->id), gie->path);
				printf("fatal: git-write-tree: error building trees\n");
				err = 0;
				goto out;
			}
		}
	}

	git_oid oid;
	if ((err = git_index_write_tree_to(&oid, idx, repo)) != GIT_OK)
		goto out;

	printf("%s\n", git_oid_tostr(sha1buf, GIT_OID_HEXSZ+1, &oid));

	rc = EXIT_SUCCESS;
out:
	if(err != GIT_OK)
		libgit_error();
	if (repo_idx) git_index_free(repo_idx);
	if (idx) git_index_free(idx);
	return rc;
}

