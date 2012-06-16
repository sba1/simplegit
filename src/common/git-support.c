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
#ifndef __amigaos4__
	execvp(git_argv[0], git_argv);
	die_errno("Failed to fallback to git.");
#endif
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
