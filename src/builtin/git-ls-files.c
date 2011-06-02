#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <git2.h>
#include "errors.h"
#include "git-support.h"
#include "repository.h"
#include "utils.h"
#include "git-parse-mode.h"
#include "strbuf.h"

/*
 * => see http://www.kernel.org/pub/software/scm/git/docs/git-ls-files.html
 * 
 * Works :
 * nothing, there's an inner bug in libgit2
 * git ls-files --stage
 * 
 */

int cmd_ls_files(int argc, const char **argv)
{
	/* Delete the following line once git tests pass */
	please_git_do_it_for_me();
	
	int show_cached = 1;

	/* options parsing */
	if (argc > 1) {
		if (argc > 2)
			please_git_do_it_for_me();
		
		if (strcmp(argv[1], "--stage") == 0 || strcmp(argv[1], "-s") == 0)
			show_cached = 0;
		else if (strcmp(argv[1], "--cached") == 0 || strcmp(argv[1], "-c") == 0)
			show_cached = 1;
		else
			please_git_do_it_for_me();
	}
	
	
	git_repository *repo = get_git_repository();
	
	git_index *index_cur;
	int e = git_index_open_inrepo(&index_cur, repo);
	if (e) libgit_error();
	
	char *buf = (char*)xmalloc(GIT_OID_HEXSZ+1);
	
	const char *prefix = get_git_prefix();
	size_t prefix_len = strlen(prefix);
	
	for (unsigned i = 0; i < git_index_entrycount(index_cur); i++) {
		git_index_entry *gie = git_index_get(index_cur, i);

		if (prefixcmp(gie->path, prefix))
			continue;
		
		if( show_cached )
			printf("%s\n", gie->path + prefix_len);
		else
			//TO-DO ! use git_index_entry_get_stage (pull request sent)
			printf("%06o %s %i\t%s\n", gie->mode, git_oid_to_string(buf, GIT_OID_HEXSZ+1, &gie->oid), 0/*git_index_entry_get_stage(gie)*/, gie->path + prefix_len);
	}

	free(buf);
	git_index_free(index_cur);
	
	return EXIT_SUCCESS;
}
