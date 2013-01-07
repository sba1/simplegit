#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <git2.h>

#include "errors.h"
#include "git-ls-tree.h"
#include "git-support.h"
#include "repository.h"
#include "quote.h"

int cmd_ls_tree(git_repository *repo, int argc, char **argv)
{
	int err;
	int rc = EXIT_FAILURE;

	if (argc != 2)
	{
		please_git_do_it_for_me();
		goto out;
	}

	/* Find the tree */
	git_tree *tree;
	git_oid oid_tree;
	
	if ((err = git_oid_fromstr(&oid_tree, (const char *)argv[1])) != GIT_OK)
		goto out;

	if ((err = git_tree_lookup(&tree, repo, &oid_tree)) != GIT_OK)
		goto out;
#if 0
	switch (err) {
//		case GIT_EINVALIDTYPE:
//			die("not a tree object");
		case GIT_ENOTFOUND: fprintfdie("Not a valid object name %s", argv[1]);
		case GIT_OK: break;
		default:
			goto out;
	}
#endif
	unsigned int i;
	unsigned int max = git_tree_entrycount(tree);
	
	for (i = 0; i < max; ++i)
	{
		const git_tree_entry *tree_entry;
		const git_oid *entry_oid;
		const char *name_entry;
		git_otype type_entry_tree;
		char buf[GIT_OID_HEXSZ + 1];

		/* Get the tree entry */
		tree_entry = git_tree_entry_byindex(tree, i);

		/* Get the tree entry type */
		type_entry_tree = git_tree_entry_type(tree_entry);

		/* Get the tree entry name */
		name_entry = git_tree_entry_name(tree_entry);

		/* Get the oid of a tree entry */
		entry_oid = git_tree_entry_id (tree_entry);

		printf("%06o %s %s\t",git_tree_entry_filemode(tree_entry), git_object_type2string(type_entry_tree),
			git_oid_tostr(buf, sizeof(buf), entry_oid));
		write_name_quoted(name_entry, stdout, '\n');
	}

	git_tree_free(tree);
	rc = EXIT_SUCCESS;
out:
	if (err)
		libgit_error();

	return rc;
}
