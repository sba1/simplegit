#include <stdlib.h>
#include "repository.h"
#include "errors.h"
#include "git-support.h"

static git_repository *repository = NULL;
static char prefix[PATH_MAX];
static int prefix_loaded = 0;

git_repository* get_git_repository() {
	if (repository == NULL) {
		const char *argv[]= {"git", "rev-parse", "--git-dir", NULL};
		char *repository_path = please_git_help_me(argv);

		if (git_repository_open(&repository, repository_path)) {
			libgit_error();
		}

		free(repository_path);
	}

	return repository;
}

const char *get_git_prefix() {
	if (!prefix_loaded) {
		const char *argv[]= {"git", "rev-parse", "--show-prefix", NULL};
		char *new_prefix = please_git_help_me(argv);

		if (sizeof(new_prefix) > sizeof(prefix)) {
			die("The given prefix is too long.\n");
		}


		strcpy(prefix, new_prefix);
		free(new_prefix);
	}

	return prefix;
}

void free_repository() {
	if (repository == NULL)
		return;

	git_repository_free(repository);
}
