#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <git2.h>
#include "errors.h"
#include "git-commit-tree.h"
#include "git-support.h"
#include "repository.h"

int cmd_commit_tree(int argc, const char **argv)
{
	//please_git_do_it_for_me();
	if (argc != 2)
		please_git_do_it_for_me();
	char *author_name = getenv("GIT_AUTHOR_NAME");
	char *author_email = getenv("GIT_AUTHOR_EMAIL");
// 	char *author_date = getenv("GIT_AUTHOR_DATE");
	char *committer_name = getenv("GIT_COMMITTER_NAME");
	char *committer_email = getenv("GIT_COMMITTER_EMAIL");
// 	char *committer_date = getenv("GIT_COMMITTER_DATE");
	
	if ((author_name == NULL) | (author_email == NULL)
		| (committer_name == NULL) | (committer_email == NULL) )
	{
		please_git_do_it_for_me();
	}
// 	(author_date == NULL)(committer_date == NULL)
	int e;
	git_repository *repo = get_git_repository();
	git_oid tree_oid;
	
	if (git_oid_mkstr(&tree_oid, (const char *)argv[argc-1]))
		libgit_error();

	/* Lookup if the object is a tree or not */
	git_tree *tree;
	e = git_tree_lookup(&tree, repo, &tree_oid);
	if (e) {
		if (e == GIT_EINVALIDTYPE || e == GIT_ENOTFOUND) {
			error("Tree object not found");
		} else {
			libgit_error();
		}
	}
	/* Lookup for parents */
// 	git_reference *ref;
// 	e= git_reference_lookup (&ref, repo, "HEAD");
// 	if(e) libgit_error();
// 	
// 	git_reference *resolved_ref;
// 	e = git_reference_resolve(&resolved_ref, ref);
// 	if(e) libgit_error();
	
// 	const git_oid *parent_oid = git_reference_oid(ref);

// 	char *timestamp_author = NULL;
// 	char *offset_author = NULL;
// 	
// 	int i = 1;
// 	if (*author_date == '<') {
// 		while ((*author_date)+i != '>') {
// 			strcat(timestamp_author, author_date+i);
// 		}
// 		i++;
// 		if ((*author_date+i) == '<') {
// 			while ((*author_date)+i != '>') {
// 				strcat(offset_author, author_date+i);
// 			}
// 		}	
// 	}
// 	int offset = atoi(offset_author+1);
// 
// 	char *timestamp_committer = NULL;
// 	char *offset_committer = NULL;
// 	i = 1;
// 	if (*committer_date == '<') {
// 		while ((*committer_date)+i != '>') {
// 			strcat(timestamp_author, committer_date+i);
// 		}
// 		i++;
// 		if ((*committer_date+i) == '<') {
// 			while ((*committer_date)+i != '>') {
// 				strcat(offset_committer, committer_date+i);
// 			}
// 		}
// 	}

	git_signature *author_signature = git_signature_new(author_name, author_email, 1306829480, 200);
	if(author_signature == NULL) printf("signature nulle\n");
	git_signature *committer_signature = git_signature_new(committer_name, committer_email, 1306829481, 200);
	if(committer_signature == NULL) printf("signature nulle\n");
	
	git_oid commit_oid;
	const git_oid *parent_oids = NULL;
	printf("Vous etes là avant la creation\n");
	e = git_commit_create(&commit_oid,
				repo,
				NULL,
				author_signature,
				committer_signature,
				"Hello tree",
				&tree_oid,
				0,
				&parent_oids);
	printf("Vous etes là après la création\n");
	if (e) libgit_error();

	/* Print to stdout */
	char *commit_oid_string = malloc(sizeof(char)*41);
	git_oid_fmt(commit_oid_string, &commit_oid);
	commit_oid_string[40] = '\0';
	printf("ICI %s\n",commit_oid_string);
	
	/*Close the commit*/
	git_commit *commit;
	e = git_commit_lookup(&commit, repo, &commit_oid);
	if (e) libgit_error();
	git_commit_close(commit);
	git_signature_free(author_signature);
	git_signature_free(committer_signature);
	return 0;
}