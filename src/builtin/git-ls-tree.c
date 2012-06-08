#include <stdlib.h>
#include <stdio.h>
#include "errors.h"
#include <git2.h>
#include <assert.h>
#include "git-ls-tree.h"
#include "git-support.h"
#include "repository.h"
#include "quote.h"

int cmd_ls_tree(int argc, const char **argv)
{
	please_git_do_it_for_me();

	if (argc != 2)
		please_git_do_it_for_me();

	int e;
	/* Find the current repository */
	git_repository *repo = get_git_repository();
	
	/*Find the tree*/
	git_tree *tree;
	git_oid oid_tree;
	
	e = git_oid_fromstr(&oid_tree, (const char *)argv[1]);

	if (e == GIT_ENOTOID) {
		//libgit do not handle extended sha1 expressions for now
		//so we fallback to git if not found.
		please_git_do_it_for_me();
	} else if (e < GIT_SUCCESS) {
		libgit_error();
	}
	
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
	
	for (i = 0; i < max; ++i) {
		/* Get the tree entry */
		const git_tree_entry *tree_entry;
		tree_entry = git_tree_entry_byindex(tree, i);
		assert(tree_entry);

		/* Get the tree entry type */
		git_otype type_entry_tree;
		type_entry_tree = git_tree_entry_type(tree_entry);

		/* Get the tree entry name */
		const char *name_entry;
		name_entry = git_tree_entry_name(tree_entry);

		/* Get the oid of a tree entry */
		const git_oid *entry_oid = git_tree_entry_id (tree_entry);

		printf("%06o %s %s\t",git_tree_entry_attributes(tree_entry), git_object_type2string(type_entry_tree),
			git_oid_tostr(buf, GIT_OID_HEXSZ + 1, entry_oid));

		write_name_quoted(name_entry, stdout, '\n');
	}

	git_tree_close(tree);

	return EXIT_SUCCESS;
}
