#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <git2.h>
#include "errors.h"
#include "git-checkout.h"
#include "git-support.h"


int cmd_checkout(int argc, const char **argv) 
{
	/* Delete the following line once gits tests pass */
	please_git_do_it_for_me();

	if (argc != 1)
		please_git_do_it_for_me();
	
	git_index *index;
	git_repository *repo;
	git_index_entry *index_entry;
	git_oid id;
	
	 /* Open the repository */
	if (git_repository_open(&repo, argv[argc-1])) {
		libgit_error();
	}

	/* Get the Index file of a Git repository */
	if (git_repository_index(&index,repo)) {
		libgit_error();
	}
	
	/* Find the first index of any entries which point to given path in the Git index */
	if (git_index_find (index, argv[argc-1])) {
		libgit_error();
	}

	int i;

	/* get a pointer to one of the entries in the index */
	index_entry = git_index_get(index, i);
	if (index_entry == NULL)
		printf("Out of bound");
	else
		id = index_entry->oid;
	(void)id;
	
	git_reference *symbolic_ref;
	if (git_reference_lookup(&symbolic_ref, repo, "HEAD"))
		libgit_error();

	git_reference *direct_ref;
	if (git_reference_resolve(&direct_ref, symbolic_ref))
		libgit_error();

	const git_oid *oid;
	oid = git_reference_oid(direct_ref);
	if (oid == NULL) {
		printf("Internal error: reference is not direct\n");
		return EXIT_FAILURE;
	}
	
	git_tree *tree;
	/* Lookup a tree object from the repository */
	if (git_tree_lookup(&tree, repo, oid))
		libgit_error();
	
	/* Update the index ?? */
	if (git_index_read(index))
		libgit_error();
	
	git_index_free(index);
	git_tree_close(tree);
	
	return EXIT_SUCCESS;
}
