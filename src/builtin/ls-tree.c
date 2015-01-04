#include "ls-tree.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <git2.h>

#include "errors.h"
#include "git-support.h"
#include "quote.h"

/**
 * Print the given tree entry.
 *
 * @param root
 * @param tree_entry
 */
static void print_tree_entry(const char *root, const git_tree_entry* tree_entry)
{
	const git_oid* entry_oid;
	const char* name_entry;
	char buf[GIT_OID_HEXSZ + 1];
	git_otype type_entry_tree;
	char buf2[1024];

	/* Get the tree entry type */
	type_entry_tree = git_tree_entry_type(tree_entry);

	/* Get the tree entry name */
	name_entry = git_tree_entry_name(tree_entry);

	/* Get the oid of a tree entry */
	entry_oid = git_tree_entry_id(tree_entry);

	printf("%06o %s %s\t", git_tree_entry_filemode(tree_entry),
			git_object_type2string(type_entry_tree),
			git_oid_tostr(buf, sizeof(buf), entry_oid));
	snprintf(buf2,sizeof(buf2),"%s%s",root?root:"",name_entry);
	write_name_quoted(buf2, stdout, '\n');
}

struct ls_tree_options
{
	int show_tree_entries;
	int recurse;
};

static int cb(const char *root, const git_tree_entry *entry, void *payload)
{
	struct ls_tree_options *pl;

	pl = (struct ls_tree_options*)payload;

	if (pl->show_tree_entries || git_tree_entry_type(entry) != GIT_OBJ_TREE)
		print_tree_entry(root,entry);
	return 0;
}

int cmd_ls_tree(git_repository *repo, int argc, char **argv)
{
	int i;
	int err;
	int max;
	int rc = EXIT_FAILURE;
	char *path = "";

	struct ls_tree_options opts;
	memset(&opts,0,sizeof(opts));

	for (i=1;i<argc;i++)
	{
		if (!strcmp("-r",argv[i])) opts.recurse = 1;
		else if (!strcmp("-t",argv[i])) opts.show_tree_entries = 1;
		else if (argv[i][0] != '-')
		{
			path = argv[i];
			break;
		} else
		{
			fprintf(stderr,"Unknown option \"%s\"\n",argv[i]);
		}
	}

	/* Find the tree */
	git_tree *tree;
	git_oid oid_tree;
	
	if ((err = git_oid_fromstr(&oid_tree, path)) != GIT_OK)
		goto out;

	if ((err = git_tree_lookup(&tree, repo, &oid_tree)) != GIT_OK)
		goto out;

	if (!opts.recurse)
	{
		max = git_tree_entrycount(tree);
		for (i = 0; i < max; ++i)
		{
			const git_tree_entry *tree_entry;

			/* Get the tree entry */
			tree_entry = git_tree_entry_byindex(tree, i);

			print_tree_entry("",tree_entry);
		}
	} else
	{
		git_tree_walk(tree,GIT_TREEWALK_PRE,cb,&opts);
	}

	git_tree_free(tree);
	rc = EXIT_SUCCESS;
out:
	if (err)
		libgit_error();

	return rc;
}
