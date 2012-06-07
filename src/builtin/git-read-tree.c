#include <stdlib.h>
#include <stdio.h>
#include "errors.h"
#include <git2.h>
#include "git-ls-tree.h"
#include "git-support.h"
#include "repository.h"
#include "utils.h"


int e;
git_index *index_cur;
git_repository *repo;

void add_tree_to_index(git_tree * tree, const char * prefix) {
	for (size_t i = 0; i < git_tree_entrycount(tree); i++) {
		/* Get the tree entry */
		const git_tree_entry *tree_entry;
		tree_entry = git_tree_entry_byindex(tree,(unsigned int)i);
		
		if (tree_entry == NULL) {
			printf("Tree entry not found");
			libgit_error();
		}

		/* Get the oid of a tree entry */
		const git_oid *entry_oid = git_tree_entry_id (tree_entry);

		/* is a sub directory ? */
		git_tree * subtree;
		if (git_tree_lookup(&subtree, repo, entry_oid) == 0) {
			char * subprefix = xmalloc(sizeof(char)*1024);
			const char * dirname = git_tree_entry_name (tree_entry);
			git2__joinpath(subprefix, prefix, dirname);
			add_tree_to_index(subtree, subprefix);
			continue;
		}
		
		char * completename = xmalloc(sizeof(char)*1024);
		git2__joinpath(completename, prefix, git_tree_entry_name (tree_entry));
		git_index_entry source_entry = {
			{0,0},//git_index_time 	ctime
			{0,0},//git_index_time 	mtime
			0,//unsigned int 	dev
			0,//unsigned int 	ino
			git_tree_entry_attributes(tree_entry),//unsigned int 	mode
			0,//unsigned int 	uid
			0,//unsigned int 	gid
			0,//git_off_t 	file_size
			*entry_oid,
			0,
			0,//unsigned short 	flags_extended
			completename
		};
		
		
		git_index_append2(index_cur, &source_entry);
		
	}
}

int cmd_read_tree(int argc, const char **argv)
{
	please_git_do_it_for_me();
	if (argc != 2)
		please_git_do_it_for_me();

	/* Find the current repository */
	repo = get_git_repository();

	/*Find the tree*/
	git_tree *tree;
	git_oid oid_tree;

	switch (git_oid_fromstr(&oid_tree, (const char *)argv[argc-1])) {
		case GIT_ENOTOID:
			please_git_do_it_for_me();
			break;
		case GIT_SUCCESS:
			break;
		default:
			libgit_error();
	}
	
	e = git_tree_lookup(&tree, repo, &oid_tree);
	if (e) {
		if (e == GIT_EINVALIDTYPE || e == GIT_ENOTFOUND) {
			error("Tree object not found");
		} else {
			libgit_error();
		}
	}

	/* Open the index */
	if (git_repository_index(&index_cur, repo) < 0)
		libgit_error();

	/* Clear the index */
	git_index_clear(index_cur);

	/* add objects of the tree to the index */
	add_tree_to_index(tree, "");

	/* write the index */
	git_index_write(index_cur);

	return EXIT_SUCCESS;
}
