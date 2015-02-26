#include "commit-tree.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "date.h"
#include "environment.h"
#include "errors.h"
#include "git-support.h"
#include "git.h"
#include "strbuf.h"

int cmd_commit_tree(git_repository *repo, int argc, char **argv)
{
	int err = 0;
	git_oid tree_oid;
	char *tree_arg = NULL;
	char commit_oid_string[GIT_OID_HEXSZ+1];
	git_oid commit_oid;

	struct strbuf buffer = STRBUF_INIT;
	git_signature *author_signature = NULL;
	git_signature *committer_signature = NULL;
	git_tree *tree = NULL;
	git_commit **parents = NULL;

	int i,p;
	int rc = EXIT_FAILURE;
	int num_parents = 0;

	for (i=1;i<argc;i++)
	{
		if (!strcmp(argv[i],"-p"))
		{
			if (++i < argc)
			{
				num_parents++;
			} else
			{
				fprintf(stderr,"Option -p misses argument\n");
				goto out;
			}
		}
		else if (argv[i][0] == '-')
		{
			fprintf(stderr,"Unknown option \"%s\"\n",argv[i]);
			goto out;
		} else
		{
			if (!tree_arg) tree_arg = argv[i];
			else
			{
				fprintf(stderr,"Only a single tree for is accepted for committing\n");
				goto out;
			}
		}
	}

	if ((err = sgit_get_author_signature(repo, &author_signature)) != GIT_OK)
		goto out;

	if ((err = sgit_get_committer_signature(repo, &committer_signature)) != GIT_OK)
		goto out;

	if (!(parents = malloc(sizeof(*parents)*num_parents)))
	{
		fprintf(stderr,"Not enough memory!\n");
		goto out;
	}

	p = 0;
	for (i=1;i<argc;i++)
	{
		if (!strcmp(argv[i],"-p"))
		{
			git_oid parent_oid;

			/* This has to exists */
			i++;
			if ((err = git_oid_fromstr(&parent_oid,argv[i])) != GIT_OK)
				goto out;
			if ((err = git_commit_lookup_prefix(&parents[p],repo,&parent_oid,strlen(argv[i]))) != GIT_OK)
				goto out;
			p++;
		}
	}

	/* Sanitize parents (remove duplicates). Use a dump O(n^2) algo.
	 * This is okay as usually the number of parents is small */
	for (p=0;p<num_parents;p++)
	{
		for (i=p+1;i<num_parents;i++)
		{
			const git_oid *poid = git_commit_id(parents[p]);
			const git_oid *ioid = git_commit_id(parents[i]);

			if (!git_oid_cmp(poid,ioid))
			{
				git_commit_free(parents[p]);
				parents[p] = NULL;
			}
		}
	}
	for (p=0,i=0;p<num_parents;p++)
	{
		if (parents[p])
		{
			if (i != p)
				parents[i] = parents[p];
			i++;
		}
	}
	num_parents = i;

	if ((err = git_oid_fromstr(&tree_oid,tree_arg)) != GIT_OK)
		goto out;
	if ((err = git_tree_lookup_prefix(&tree,repo,&tree_oid,strlen(tree_arg))) != GIT_OK)
		goto out;

	if (strbuf_read(&buffer, 0, 0) < 0)
	{
		fprintf(stderr,"Failed to read changelog from stdin!\n");
		goto out;
	}

	if ((err = git_commit_create(&commit_oid, repo, NULL, author_signature, committer_signature,
				NULL, buffer.buf, tree, num_parents, (const git_commit**)parents)) != GIT_OK)
		goto out;

	printf("%s\n", git_oid_tostr(commit_oid_string, sizeof(commit_oid_string), &commit_oid));

	rc = EXIT_SUCCESS;
out:
	if (err) libgit_error();
	if (tree) git_tree_free(tree);
	if (parents)
	{
		for (i=0;i<num_parents;i++)
			git_commit_free(parents[i]);
		free(parents);
	}
	if (author_signature) git_signature_free(author_signature);
	if (committer_signature) git_signature_free(committer_signature);
	strbuf_release(&buffer);
	return rc;
}
