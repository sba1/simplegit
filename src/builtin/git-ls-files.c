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
#include "index-entry.h"

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

	/* options parsing */
	if (argc != 2)
		please_git_do_it_for_me();
	
	if( strcmp(argv[1], "--stage") != 0 && strcmp(argv[1], "-s") != 0)
		please_git_do_it_for_me();
	
	git_repository *repo = get_git_repository();
	
	git_index *index_cur;
	int e = git_index_open_inrepo(&index_cur, repo);
	if (e == GIT_ENOTIMPLEMENTED) {
		please_git_do_it_for_me();
	} else if (e) {
		libgit_error();
	}
	
	char *buf = (char*)xmalloc(GIT_OID_HEXSZ+1);
	
	const char *prefix = get_git_prefix();
	
	for( unsigned i = 0; i < git_index_entrycount(index_cur); i++) {
		git_index_entry *gie = git_index_get(index_cur, i);

		if (prefixcmp(gie->path, prefix)) {
			continue;
		}
		
		git_tag * tag;
		e = git_tag_lookup(&tag,repo,&gie->oid);
		if (e == GIT_ENOTIMPLEMENTED) {
			please_git_do_it_for_me();
		} else if (e) {
			/* no problem if tag was not found */
			if (e == GIT_EINVALIDTYPE) {
				/* Shouldn't libgit2 return NOTIMPLEMENTED for submodules ? */
				please_git_do_it_for_me();
			} else if (e != GIT_ENOTFOUND) {
				libgit_error(e);
			}
		} else {
			printf("%s ", git_tag_name(tag));
		}
		
		printf("%06o %s %i\t%s\n", gie->mode, git_oid_to_string(buf, GIT_OID_HEXSZ+1, &gie->oid), get_stage(gie), gie->path);
	}

	free(buf);
	git_index_free(index_cur);
	
	return 0;
}
