#include "commit.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <git2.h>

#include "common.h"
#include "date.h"
#include "environment.h"
#include "errors.h"
#include "git-support.h"
#include "git.h"
#include "strbuf.h"

int cmd_commit(git_repository *repo, int argc, char **argv)
{
	int err = 0;
	git_reference *head = NULL;
	git_reference *branch = NULL;
	git_commit *parent = NULL;
	git_oid tree_oid;
	git_oid commit_oid;
	git_commit *commit = NULL;

	git_index *idx = NULL;

	git_signature *author_signature = NULL;
	git_signature *committer_signature = NULL;
	git_tree *tree = NULL;
	git_commit **parents = NULL;
	const char *message = NULL;

	int i;
	int rc = EXIT_FAILURE;
	int num_parents = 0;

	for (i=1;i<argc;i++)
	{
		if (!strcmp(argv[i],"-m"))
		{
			if (++i < argc)
			{
				message = argv[i];
			} else
			{
				fprintf(stderr,"Option -m misses argument\n");
				goto out;
			}
		}
		else if (argv[i][0] == '-')
		{
			fprintf(stderr,"Unknown option \"%s\"\n",argv[i]);
			goto out;
		}
	}

	if (!message)
	{
		fprintf(stderr,"A commit message is required (-m option)!\n");
		goto out;
	}

	if ((git_repository_head(&head,repo)) == GIT_OK)
		git_reference_peel((git_object**)&parent,head,GIT_OBJ_COMMIT);

	if ((err = sgit_get_author_signature(&author_signature)) != GIT_OK)
		goto out;

	if ((err = sgit_get_committer_signature(&committer_signature)) != GIT_OK)
		goto out;

	if ((num_parents = !!parent))
	{
		if (!(parents = malloc(sizeof(*parents)*num_parents)))
		{
			fprintf(stderr,"Not enough memory!\n");
			goto out;
		}
		parents[0] = parent;
	}

	/* Write index as tree */
	if ((err = git_repository_index(&idx,repo)) != GIT_OK)
		goto out;
	if (git_index_entrycount(idx) == 0)
	{
		fprintf(stderr,"Nothing to commit!\n");
		goto out;
	}
	if ((err = git_index_write_tree_to(&tree_oid, idx, repo)) != GIT_OK)
		goto out;
	if ((err = git_tree_lookup(&tree,repo,&tree_oid)) != GIT_OK)
		goto out;

	/* Write tree as commit */
	if ((err = git_commit_create(&commit_oid, repo, "HEAD", author_signature, committer_signature,
				NULL, message, tree, num_parents, (const git_commit**)parents)) != GIT_OK)
		goto out;

	rc = EXIT_SUCCESS;
out:
	if (err) libgit_error();
	if (head) git_reference_free(head);
	if (tree) git_tree_free(tree);
	if (idx) git_index_free(idx);
	if (parents)
	{
		for (i=0;i<num_parents;i++)
			git_commit_free(parents[i]);
		free(parents);
	}
	if (author_signature) git_signature_free(author_signature);
	if (committer_signature) git_signature_free(committer_signature);
	if (commit) git_commit_free(commit);
	if (branch) git_reference_free(branch);
	return rc;
}
