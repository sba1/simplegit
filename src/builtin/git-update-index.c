#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include <git2.h>

#include "errors.h"
#include "git-support.h"
#include "repository.h"

#define UNSUPORTED_OPTIONS_NUM 25

const char * unsuported_options[UNSUPORTED_OPTIONS_NUM] = 
{
	"--nonsense", "-h",
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
	
	int filec = argc - 1;
	const char **filev = argv + 1;
	
	int only_update_entry = 1;
	
	if (strcmp(argv[1], "--add") == 0) {
		only_update_entry = 0;
		filec--;
		filev++;
	}

	
	if (strcmp(filev[0], "--") == 0) {
		filec--;
		filev++;
	} else {
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
		strcpy(complete_path, get_git_prefix());
		strcat(complete_path, filev[i]);
		

		if (only_update_entry) {
			if (git_index_find(index_cur, complete_path) < 0) {
				printf("error: %s: cannot add to the index - missing --add option?\n", filev[i]);
				printf("fatal: Unable to process path %s\n", filev[i]);
				return 1;
			}
		}

		int e = git_index_add_from_workdir(index_cur, complete_path);
		

		if (e != 0)
			libgit_error();
	}

	
	if (exit_status == EXIT_SUCCESS)
		git_index_write(index_cur);

	

	return exit_status;
}

