#include <stdlib.h>
#include <stdio.h>
#include <git2.h>
#include <assert.h>
#include "errors.h"
#include "git-ls-tree.h"
#include "git-support.h"
#include "repository.h"

int cmd_ls_tree(int argc, const char **argv)
{
<<<<<<< HEAD
	//please_git_do_it_for_me();
=======
>>>>>>> 0fe75bbb3d4d0e0f7df2a37f3ba010a5a2043ede
	if (argc != 2)
		please_git_do_it_for_me();

	int e;
	/* Find the current repository */
	git_repository *repo = get_git_repository();
	
	/*Find the tree*/
	git_tree *tree;
	git_oid oid_tree;
	
<<<<<<< HEAD
	if (git_oid_mkstr(&oid_tree, (const char *)argv[argc-1])) 
		libgit_error();
		
=======
	e = git_oid_mkstr(&oid_tree, (const char *)argv[1]);

	if (e == GIT_ENOTOID) {
		//libgit do not handle extended sha1 expressions for now
		//so we fallback to git if not found.
		please_git_do_it_for_me();
	} else if (e < GIT_SUCCESS) {
		libgit_error();
	}
>>>>>>> 0fe75bbb3d4d0e0f7df2a37f3ba010a5a2043ede
	
	e = git_tree_lookup(&tree, repo, &oid_tree);
	switch (e) {
		case GIT_EINVALIDTYPE:
			die("not a tree object");
		case GIT_ENOTFOUND:
			die("Not a valid object name %s", argv[1]);
		case GIT_SUCCESS:
			break;
		default:
			libgit_error();
	}

	char buf[GIT_OID_HEXSZ + 1];
	unsigned int i;
	unsigned int max = git_tree_entrycount(tree);
	
<<<<<<< HEAD
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
=======
	for (i = 0; i < max; ++i) {
		/* Get the tree entry */
		const git_tree_entry *tree_entry;
		tree_entry = git_tree_entry_byindex(tree, i);
		assert(tree_entry);

		/* Get the tree entry type */
		git_otype type_entry_tree;
		type_entry_tree = git_tree_entry_type(tree_entry);
>>>>>>> 0fe75bbb3d4d0e0f7df2a37f3ba010a5a2043ede

		const char *name_entry;
		name_entry = git_tree_entry_name(tree_entry);
<<<<<<< HEAD
		
		const git_oid *entry_oid = git_tree_entry_id(tree_entry);
		
 		printf("%06o %s %s\t%s\n",git_tree_entry_attributes(tree_entry),git_object_type2string(type_entry_tree),
 		       git_oid_to_string(buf, GIT_OID_HEXSZ+1, entry_oid), name_entry);

	}
	git_tree_close(tree);
	git_object_close(object_entry_tree);
	free(buf);
=======

		/* Get the oid of a tree entry */
		const git_oid *entry_oid = git_tree_entry_id (tree_entry);

		printf("%06o %s %s\t%s\n",git_tree_entry_attributes(tree_entry), git_object_type2string(type_entry_tree),
			git_oid_to_string(buf, GIT_OID_HEXSZ + 1, entry_oid), name_entry);
	}

	git_tree_close(tree);
>>>>>>> 0fe75bbb3d4d0e0f7df2a37f3ba010a5a2043ede

	return EXIT_SUCCESS;
}
