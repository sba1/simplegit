#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <git2.h>
#include "errors.h"
#include "git-support.h"
#include "repository.h"
#include "utils.h"
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


/* transform an oid mode in a beautiful string */
/*char *parse_oid_mode(unsigned int mode) {
	static char buff[sizeof(char)*7];
	buff[6] = '\0';
	buff[5] = '0' + (mode & 0x7);
	buff[4] = '0' + ((mode >> 3) & 0x7);
	buff[3] = '0' + ((mode >> 6) & 0x7);
	buff[2] = '0';
	buff[1] = '0' + ((mode >> 12) & 0x7);
	buff[0] = '0' + (mode >> 15);
	return buff;
}*/

int cmd_ls_files(int argc, const char **argv)
{
	please_git_do_it_for_me();

	/* options parsing */
	if(argc != 1)
		please_git_do_it_for_me();
	
	if( strcmp(argv[0], "--stage") != 0)
		please_git_do_it_for_me();
	
	
	git_repository *repo = get_git_repository();
	
	git_index *index_cur;
	if (git_index_open_inrepo(&index_cur, repo))
		libgit_error();

	char *buf = (char*)xmalloc(GIT_OID_HEXSZ+1);
	
	const char *prefix = get_git_prefix();

	for( unsigned i = 0; i < git_index_entrycount(index_cur); i++) {
		git_index_entry *gie = git_index_get(index_cur, i);

		if (prefixcmp(gie->path, prefix)) {
			continue;
		}
		
		git_tag * tag;
		if(git_tag_lookup(&tag,repo,&gie->oid) == 0) {
			printf("%s ", git_tag_name(tag));
		}
		
		printf("%06o %s %i\t%s\n", gie->mode, git_oid_to_string(buf, GIT_OID_HEXSZ+1, &gie->oid), get_stage(gie), gie->path);
	}

	free(buf);
	git_index_free(index_cur);
	
	return 0;
}
