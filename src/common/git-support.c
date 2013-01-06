#include <unistd.h>
#include "git-support.h"
#include "run-command.h"
#include "utils.h"
#include "errors.h"

char *please_git_help_me(const char **argv) {
#ifdef __amigaos4__
	char *buffer = '\0';
	printf("please_git_help_me()\n");
#else
	struct child_process process;
	memset(&process, 0, sizeof(process));
	process.argv = argv;
	process.out = -1;

	run_command(&process);
	char *buffer = (char*)xmalloc(sizeof(char) * (1024 + 1));
	char *buf = buffer;

	int count = 0;
	int loaded;

	while (1) {
		loaded = xread(process.out, buffer, 1024);

		if (loaded < 0) {
			die_errno("xread from child_process");
		}

		if (loaded == 0) {
			buffer[count] = '\0';

			if (count > 0 && buffer[count - 1] == '\n') {
				buffer [ count -1 ] = '\0';
			}
			break;
		}

		count += loaded;
		buf += loaded;

		if (loaded == 1024) {
			buffer = (char*)xrealloc(buffer, sizeof(char) * (count + 1024 + 1));
		}
	}
#endif
	return buffer;
}

static char **git_argv = NULL;

void please_git_do_it_for_me() {
	fprintf(stderr,"please_git_do_it_for_me()\n");
	exit(20);
}

void git_support_register_arguments(int argc, const char **argv) {
	git_argv = (char**)xmalloc(sizeof(char*) * (argc + 1));

	git_argv[0] = (char*)xstrdup("git");

	for (int i = 1; i < argc; ++i) {
		git_argv[i] = (char*)xstrdup(argv[i]);
	}

	git_argv[argc] = NULL;
}

void git_support_free_arguments() {
	if (git_argv) {
		for (int i = 0; git_argv[i]; ++i) {
			free(git_argv[i]);
		}

		free(git_argv);
	}
}


/**
 * Create a branch with a given branch name from a reference.
 *
 * @param new_branch_ref where the reference of the new branch is stored to.
 * @param branch_ref the ref which to branch.
 * @param repo the repo
 * @param branch the name of the new branch
 * @return error code or 0 for success.
 */
int git2_create_branch_from_ref(git_reference **new_branch_ref, git_reference *branch_ref, git_repository* repo, const char *branch)
{
	int err;

	git_object *commit;
	git_commit *comm;

	if ((err = git_reference_peel(&commit, branch_ref, GIT_OBJ_COMMIT)) != 0)
		goto out;

	if ((err = git_commit_lookup(&comm, repo, git_object_id(commit))) != 0)
		goto out;

	err = git_branch_create(new_branch_ref, repo, branch, comm, 0);
out:
	return err;
}
