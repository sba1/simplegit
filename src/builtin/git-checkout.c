#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <git2.h>
#include "errors.h"
#include "git-checkout.h"
#include "git-support.h"


int cmd_checkout(int argc, const char **argv) 
{
	if(argc != 2)
		please_git_do_it_for_me();
	
	int e,i;
	git_index *index;
	git_repository *repo;
	git_index_entry *index_entry;
	git_oid id;
	
	 /* Open the repository */
	e = git_repository_open(&repo, argv[argc-1]);
	if (e) libgit_error(e);

	/* Get the Index file of a Git repository */
	e = git_repository_index(&index,repo);
	if(e) libgit_error(e);
	
	/* Find the first index of any entries which point to given path in the Git index */
	i = git_index_find (index, argv[argc-1]);
	if(i) libgit_error(e);
	else {
	  /* get a pointer to one of the entries in the index */
		index_entry = git_index_get(index, i);
		if(index_entry == NULL) printf("Out of bound");
		else id = index_entry->oid;
		(void)id;
	}
	
	git_reference *symbolic_ref;
	if(git_reference_lookup(&symbolic_ref, repo, "HEAD")) libgit_error(e);

	git_reference *direct_ref;
	if(git_reference_resolve(&direct_ref, symbolic_ref)) libgit_error(e);

	const git_oid *oid;
	oid = git_reference_oid(direct_ref);
	if (oid == NULL) {
		printf("Internal error: reference is not direct\n");
		return EXIT_FAILURE;
	}
	
	git_tree *tree;
	/* Lookup a tree object from the repository */
	e = git_tree_lookup(&tree, repo, oid);
	if(e) libgit_error(e);
	
	/* Update the index ?? */
	e = git_index_read(index);
	if(e) libgit_error(e);
	
	git_index_free(index);
	git_repository_free(repo);
	git_tree_close(tree);
	
	return 0;
}
