#include <stdlib.h>
#include "repository.h"
#include "errors.h"
#include "git-support.h"

static git_repository *repository=NULL;

git_repository* get_git_repository() {
	if (repository == NULL) {
		const char *argv[]= {"git", "rev-parse", "--git-dir", NULL};
		char *repository_path = please_git_help_me(argv);

		int error = git_repository_open(&repository, repository_path);

		free(repository_path);

		if (error) {
			libgit_error(error);
		}
	}

	return repository;
}
