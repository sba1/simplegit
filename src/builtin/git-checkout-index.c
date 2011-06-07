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
#include "fileops.h"




int cmd_checkout_index(int argc, const char **argv) 
{
	/* Delete the following line once gits tests pass */
	please_git_do_it_for_me();

	/* options parsing */
	if (argc != 3)
		please_git_do_it_for_me();

	
	if (!(strcmp(argv[1], "-f") == 0 && strcmp(argv[2], "-a") == 0))
		please_git_do_it_for_me();


// 	printf("lalalalalala\n");

	
	git_repository *repo = get_git_repository();
	
	git_index *index_cur;
	int e = git_repository_index(&index_cur, repo);
	if (e) libgit_error();

	
	char *buf = (char*)xmalloc(GIT_OID_HEXSZ+1);
	
// 	const char *prefix = get_git_prefix();
// 	size_t prefix_len = strlen(prefix);
	
	for (unsigned i = 0; i < git_index_entrycount(index_cur); i++) {
		git_index_entry *gie = git_index_get(index_cur, i);
// 		printf("%s\n",gie->path);
		git_odb_object * obj;
		e = git_odb_read(&obj, git_repository_database(repo), &gie->oid);
		if(e != GIT_SUCCESS)
			libgit_error();

		const void* data = git_odb_object_data(obj);
		size_t data_size = git_odb_object_size(obj);
		const char * filename = gie->path;

		int p;
		p = git2_creat_force(filename, gie->mode);
		if (p < 0) {
			printf("Error in opening file\n");
			return 1;
		}
		
		write(p, data, data_size);
		close(p);
		
		
	}

	
	free(buf);
	git_index_free(index_cur);
	
	return EXIT_SUCCESS;
}
