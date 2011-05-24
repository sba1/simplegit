#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <git2.h>
#include "errors.h"
#include "git-support.h"
#include "repository.h"
#include "utils.h"
#include "git-parse-mode.h"

/*
 * => see http://www.kernel.org/pub/software/scm/git/docs/git-ls-files.html
 * 
 * Works :
 * git ls-files --stage
 * 
 */

int cmd_ls_files(int argc, const char **argv)
{
	/* Delete the following line once git tests pass */
	please_git_do_it_for_me();

	int e;

	/* options parsing */
	if (argc != 2)
		please_git_do_it_for_me();
	
	if( strcmp(argv[1], "--stage") != 0 && strcmp(argv[1], "-s") != 0)
		please_git_do_it_for_me();
	
	git_repository *repo = get_git_repository();
	
	git_index *index_cur;
	e = git_index_open_inrepo(&index_cur, repo);
	if (e == GIT_ENOTIMPLEMENTED) {
		please_git_do_it_for_me();
	} else if (e) {
		libgit_error(e);
	}
	
	char *buf = (char*)xmalloc(GIT_OID_HEXSZ+1);
	
	int stage = 0;
	int we_are_in_stage = 0;
	
	for (unsigned i = 0; i < git_index_entrycount(index_cur); i++) {
		git_index_entry *gie = git_index_get(index_cur, i);
		
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
		
		/* computing the current stage */
		if (i + 1 < git_index_entrycount(index_cur)) {
			git_index_entry *gie_p1 = git_index_get(index_cur, i + 1);
			if (strcmp(gie_p1->path, gie->path) == 0) {
				we_are_in_stage = 1;
				stage++;
			} else {
				if (we_are_in_stage) {
					we_are_in_stage = 0;
					stage++;
				} else {
					stage = 0;
				}
			}
		} else {
			if (we_are_in_stage) {
				stage++;
			} else {
				stage = 0;
			}
		}
		
		
		printf("%s %s %i\t%s\n", parse_oid_mode(gie->mode),
			git_oid_to_string(buf, GIT_OID_HEXSZ + 1, &gie->oid), stage, gie->path);

	}

	free(buf);
	git_index_free(index_cur);
	
	return 0;
}
