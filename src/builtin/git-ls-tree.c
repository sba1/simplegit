#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <git2.h>
#include "errors.h"
#include "git-ls-tree.h"
#include "git-support.h"
#include "repository.h"

int cmd_ls_tree(int argc, const char **argv)
{
	if(argc != 1)
		please_git_do_it_for_me();
	
	int e;
	/* Find the current repository */
	git_repository *repo = get_git_repository();
	
	/*Find the tree*/
	git_tree *tree;
	git_oid oid_tree;
	
	e = git_oid_mkstr(&oid_tree, (const char *)argv[argc-1]);
	if(e) libgit_error(e);
	
	e = git_tree_lookup(&tree, repo, &oid_tree);
	if(e == GIT_ENOTOID) {
		libgit_error(e);
	}
	char *buf = malloc(GIT_OID_HEXSZ+1);
	size_t i;
	if(git_tree_entrycount(tree) == 0) printf("No entry tree in this tree");
	for(i=0; i < git_tree_entrycount(tree); i++) {
		/* Get the tree entry */
		const git_tree_entry *tree_entry;
		tree_entry = git_tree_entry_byindex(tree,(unsigned int)i);
		
		if(tree_entry == NULL) printf("Tree entry not found");

		/* Get the tree entry name */
		const char *name_entry;
		name_entry = git_tree_entry_name(tree_entry);
		
		/* Convert a tree entry to the git_object it points to */
		git_object *object_entry_tree;
		e = git_tree_entry_2object (&object_entry_tree, repo, tree_entry);
		if(e) libgit_error(e);
		
		/* Get the tree entry type */
		git_otype type_entry_tree;
		type_entry_tree = git_object_type(object_entry_tree);
		
		/* Get the oid of a tree entry */
		const git_oid *entry_oid = git_tree_entry_id (tree_entry);
		//const git_oid *entry_oid = git_object_id(object_entry_tree);
		
		printf("%d %s %s %s",git_tree_entry_attributes(tree_entry),git_object_type2string(type_entry_tree), git_oid_to_string(buf, GIT_OID_HEXSZ+1, entry_oid), name_entry);
		git_object_close(object_entry_tree);
	}
	git_tree_close(tree);
	return 0;
}