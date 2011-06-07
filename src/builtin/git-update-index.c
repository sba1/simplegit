#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include <git2.h>

#include "errors.h"
#include "git-support.h"
#include "repository.h"

#define UNSUPORTED_OPTIONS_NUM 23

const char * unsuported_options[UNSUPORTED_OPTIONS_NUM] = 
{
	"--remove", "--force-remove", "--replace",
	"--refresh", "-q", "--unmerged", "--ignore-missing",
	"--cacheinfo",
	"--chmod",
	"--assume-unchanged", "--no-assume-unchanged",
	"--skip-worktree", "--no-skip-worktree",
	"--ignore-submodules",
	"--really-refresh", "--unresolve", "--again", "-g",
	"--info-only", "--index-info",
	"-z", "--stdin",
	"--verbose"
};

int cmd_update_index(int argc, const char **argv)
{
	please_git_do_it_for_me();
	
 	if (argc < 2)
		please_git_do_it_for_me();
	
	if (strcmp(argv[1], "--add") != 0)
		please_git_do_it_for_me();
	
	int filec;
	const char **filev;
	
	if (strcmp(argv[2], "--") == 0) {
		filec = argc - 3;
		filev = argv + 3;
	} else {
		filec = argc - 2;
		filev = argv +2;
		for (int i = 0; i < filec; i++) {
			for (int j = 0; j < UNSUPORTED_OPTIONS_NUM; j++) {
				if (strcmp(filev[i], unsuported_options[j]) == 0 )
					please_git_do_it_for_me();
			}
		}
	}
	
	
	int exit_status = EXIT_SUCCESS;
	
	/* Open the repo */
	git_repository *repo = get_git_repository();
	
	/* Open the index */
	git_index *index_cur;
	if (git_repository_index(&index_cur, repo) < 0)
		libgit_error();
	
	char complete_path[5000];
	for (int i = 0; i < filec; i++) {
		int stage = 0;
		strcpy(complete_path, get_git_prefix());
		strcat(complete_path, filev[i]);
		int e = git_index_add(index_cur, complete_path, stage);
		if (e != 0)
			libgit_error();
	}
	
	if (exit_status == EXIT_SUCCESS)
		git_index_write(index_cur);
	
// 	git_index_free(index_cur);
// 	git_repository_free(repo);
	
	return exit_status;
}

