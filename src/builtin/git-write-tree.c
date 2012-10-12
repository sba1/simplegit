#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "errors.h"
#include <git2.h>

#include "git-support.h"
#include "repository.h"
#include "strbuf.h"
#include "utils.h"


int cmd_write_tree(int argc, const char **argv)
{
	please_git_do_it_for_me();

	int verify_index = 1;
	if (argc == 1)
		verify_index = 1;
	else if (argc == 2 && strcmp(argv[1], "--missing-ok") == 0 )
		verify_index = 0;
	else
		please_git_do_it_for_me();
	

	char sha1buf[GIT_OID_HEXSZ + 1];

	git_repository *repo = get_git_repository();
	git_index *index_cur;
	int e = git_repository_index(&index_cur, repo);
	if(e != GIT_OK)
		libgit_error();

	/* check the index */
	if (verify_index) {
		git_odb * odb;
		git_repository_odb(&odb, repo);
		for (unsigned i = 0; i < git_index_entrycount(index_cur); i++) {
			git_index_entry *gie = git_index_get(index_cur, i);

			if (git_odb_exists(odb, &gie->oid) != 1) {
				printf("error: invalid object %06o %s for '%s'\n", gie->mode, git_oid_tostr(sha1buf, GIT_OID_HEXSZ+1, &gie->oid), gie->path);
				printf("fatal: git-write-tree: error building trees\n");
				return EXIT_FAILURE;
			}
		}
	}
	
	/* create the tree */
	git_oid oid;
	e = git_tree_create_fromindex(&oid, index_cur);
	if(e != GIT_OK)
		libgit_error();

	printf("%s\n", git_oid_tostr(sha1buf, GIT_OID_HEXSZ+1, &oid));

	return EXIT_SUCCESS;
}

