#include "repository.h"
#include "errors.h"

static git_repository *repository=NULL;

git_repository* get_git_repository() {
	if (repository == NULL) {
	
		//TO-DO ! find the proper git directory (and ark to it in
		//libgit2).
		int error = git_repository_open(&repository, ".git");

		if (error) {
			libgit_error(error);
		}
	}

	return repository;
}
