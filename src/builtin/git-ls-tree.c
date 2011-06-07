#include <stdlib.h>
#include <stdio.h>
#include <git2.h>
#include "errors.h"
#include "git-ls-tree.h"
#include "git-support.h"
#include "repository.h"

int cmd_ls_tree(int argc, const char **argv)
{
	//please_git_do_it_for_me();
	if (argc != 2)
		please_git_do_it_for_me();

	int e;
	/* Find the current repository */
	git_repository *repo = get_git_repository();
	
	/*Find the tree*/
	git_tree *tree;
	git_oid oid_tree;
	
	if (git_oid_mkstr(&oid_tree, (const char *)argv[argc-1])) 
		libgit_error();
		
	
	e = git_tree_lookup(&tree, repo, &oid_tree);
	if (e) {
		if (e == GIT_EINVALIDTYPE || e == GIT_ENOTFOUND) {
			error("Tree object not found");
		} else {
			libgit_error();
		}
	}

	char *buf = malloc(GIT_OID_HEXSZ+1);
	size_t i;
	
	if (git_tree_entrycount(tree) == 0)
		printf("No entry tree in this tree");
	
	git_object *object_entry_tree;
	for (i = 0; i < git_tree_entrycount(tree); i++) {
		
		const git_tree_entry *tree_entry;
		tree_entry = git_tree_entry_byindex(tree,(unsigned int)i);
		
		if (tree_entry == NULL)
			printf("Tree entry not found");
    
 		if (git_tree_entry_2object(&object_entry_tree, repo, tree_entry))
 		{
 			printf("Erreur là \n");
  			libgit_error();
 		}
 		
 		git_otype type_entry_tree;
 		type_entry_tree = git_object_type(object_entry_tree);

		const char *name_entry;
		name_entry = git_tree_entry_name(tree_entry);
		
		const git_oid *entry_oid = git_tree_entry_id(tree_entry);
		
 		printf("%06o %s %s\t%s\n",git_tree_entry_attributes(tree_entry),git_object_type2string(type_entry_tree),
 		       git_oid_to_string(buf, GIT_OID_HEXSZ+1, entry_oid), name_entry);

	}
	git_tree_close(tree);
	git_object_close(object_entry_tree);
	free(buf);

	return EXIT_SUCCESS;
}
