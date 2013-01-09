#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "errors.h"
#include <git2.h>
#include "git.h"
#include "git-commit-tree.h"
#include "git-support.h"
#include "repository.h"
#include "date.h"
#include "strbuf.h"
#include "environment.h"

int cmd_commit_tree(git_repository *repo, int argc, char **argv)
{
	char *author_name = NULL;
	char *author_email = NULL;
	char *author_date = NULL;
	char *committer_name = NULL;
	char *committer_email = NULL;
	char *committer_date = NULL;
	unsigned long author_timestamp = 0;
	int author_offset = 0;
	unsigned long committer_timestamp = 0;
	int committer_offset = 0;
	int err = 0;
	git_oid tree_oid;
	char *tree_arg = NULL;
	char commit_oid_string[GIT_OID_HEXSZ+1];
	git_oid commit_oid;

	struct strbuf buffer = STRBUF_INIT;
	git_signature *author_signature = NULL;
	git_signature *committer_signature = NULL;
	git_tree *tree = NULL;

	int i;
	int rc = EXIT_FAILURE;

	for (i=1;i<argc;i++)
	{
		if (argv[i][0] == '-')
		{
			fprintf(stderr,"Unknown option \"%s\"\n",argv[i]);
			goto out;
		}

		if (!tree_arg) tree_arg = argv[i];
		else
		{
			fprintf(stderr,"Only a single tree is accepted for now\n");
			goto out;
		}
	}

	author_name = getenv(GIT_AUTHOR_NAME_ENVIRONMENT);
	author_email = getenv(GIT_AUTHOR_EMAIL_ENVIRONMENT);
	author_date = getenv(GIT_AUTHOR_DATE_ENVIRONMENT);
	committer_name = getenv(GIT_COMMITTER_NAME_ENVIRONMENT);
	committer_email = getenv(GIT_COMMITTER_EMAIL_ENVIRONMENT);
	committer_date = getenv(GIT_COMMITTER_DATE_ENVIRONMENT);
	
	if (!author_name) author_name = "Dummy Author";
	if (!author_email) author_email ="dummya@dummydummydummy.zz";
	if (!committer_name) committer_name = "Dummy Committerr";
	if (!committer_email) author_email ="dummyc@dummydummydummy.zz";

	if (committer_date)
	{
		if ((parse_date_basic(author_date, &author_timestamp, &author_offset)))
		{
			fprintf(stderr,"Bad author date format\n!");
			goto out;
		}
	}

	if (author_date)
	{
		if ((parse_date_basic(committer_date, &committer_timestamp, &committer_offset)))
		{
			fprintf(stderr,"Bad committer date format\n!");
			goto out;
		}
	}

	if ((err = git_oid_fromstr(&tree_oid,tree_arg)) != GIT_OK)
		goto out;
	if ((err = git_tree_lookup_prefix(&tree,repo,&tree_oid,strlen(tree_arg))) != GIT_OK)
		goto out;
	if ((err = git_signature_new(&author_signature, author_name, author_email, author_timestamp, author_offset)) != GIT_OK)
		goto out;
	if ((err = git_signature_new(&committer_signature, committer_name, committer_email, committer_timestamp, committer_offset)) != GIT_OK)
		goto out;

	if (strbuf_read(&buffer, 0, 0) < 0)
	{
		fprintf(stderr,"Failed to read changelog from stdin!\n");
		goto out;
	}

	if ((err = git_commit_create(&commit_oid, repo, NULL, author_signature, committer_signature,
				NULL, buffer.buf, tree, 0, NULL)) != GIT_OK)
		goto out;

	printf("%s\n", git_oid_tostr(commit_oid_string, sizeof(commit_oid_string), &commit_oid));

	rc = EXIT_SUCCESS;
out:
	if (err) libgit_error();
	if (tree) git_tree_free(tree);
	if (author_signature) git_signature_free(author_signature);
	if (committer_signature) git_signature_free(committer_signature);
	strbuf_release(&buffer);
	return rc;
}
