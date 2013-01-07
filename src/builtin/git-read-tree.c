#include <stdlib.h>
#include <stdio.h>

#include <git2.h>

#include "errors.h"
#include "git-ls-tree.h"
#include "git-support.h"
#include "repository.h"
#include "utils.h"

int cmd_read_tree(git_repository *repo, int argc, char **argv)
{
	int rc = EXIT_FAILURE;
	int err = 0;
	int i;
	char *treeish;

	for (i=1;i<argc;i++)
	{
		if (argv[i][0] != '-')
		{
			treeish = argv[i];
		} else
		{
			fprintf(stderr,"Unknown option \"%s\"\n",argv[i]);
		}
	}

	git_oid oid_tree;
	git_tree *tree = NULL;
	git_index *idx = NULL;

	/* Find the tree */
	if ((err = git_oid_fromstr(&oid_tree, treeish)) != GIT_OK)
		goto out;

	if ((err = git_tree_lookup(&tree, repo, &oid_tree)) != GIT_OK)
		goto out;

	/* Open the index */
	if ((err = git_repository_index(&idx, repo)) != GIT_OK)
		goto out;

	/* Clear the index */
	git_index_clear(idx);

	if ((err = git_index_read_tree(idx,tree)) != GIT_OK)
		goto out;

	/* write the index */
	git_index_write(idx);

	rc = EXIT_SUCCESS;
out:

	if (idx) git_index_free(idx);
	if (tree) git_tree_free(tree);
	if (err) libgit_error();
	return rc;
}
