#include <stdlib.h>
#include "errors.h"
#include "repository.h"
#include "git-support.h"
#include "environment.h"

static git_repository *repository = NULL;
static char prefix[PATH_MAX];
static int prefix_loaded = 0;

git_repository* get_git_repository() {
	if (repository == NULL) {
		char discovered_path[PATH_MAX];
		char *repository_path = getenv(GIT_DIR_ENVIRONMENT);

		if (repository_path == NULL) {
			if (git_repository_discover(discovered_path, sizeof(discovered_path), "", 0, getenv(GIT_CEILING_DIRECTORIES_ENVIRONMENT)) < GIT_OK) {
				libgit_error();
			}

			repository_path = discovered_path;
		}

		if (git_repository_open(&repository, repository_path) < GIT_OK) {
			libgit_error();
		}
	}

	return repository;
}

const char *get_git_prefix() {
	if (!prefix_loaded) {
		const char *argv[]= {"git", "rev-parse", "--show-prefix", NULL};
		char *new_prefix = malloc(1);
		new_prefix[0] = '\0';
		
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
