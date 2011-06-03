#include <string.h>
#include "git.h"
#include "builtin.h"
#include "exec-cmd.h"
#include "git-support.h"
#include "run-command.h"
#include "errors.h"
#include "parse-options.h"
#include "repository.h"
#include "strbuf.h"

static const char git_usage_string[] =
	"git [--version] [--exec-path[=<path>]] [--html-path] [--man-path] [--info-path]\n"
	"           [-p|--paginate|--no-pager] [--no-replace-objects]\n"
	"           [--bare] [--git-dir=<path>] [--work-tree=<path>]\n"
	"           [-c name=value] [--help]\n"
	"           <command> [<args>]";

// original source : https://github.com/vfr-nl/git2
void free_global_resources() {
	git_support_free_arguments();
	git_exec_cmd_free_resources();
	free_repository();
}

static int handle_options(const char ***argv, int *argc) {
	int handled = 0;

	while (*argc > 0) {
		const char *cmd = (*argv)[0];
		if (cmd[0] != '-')
			break;

		/*
		 * For legacy reasons, the "version" and "help"
		 * commands can be written with "--" prepended
		 * to make them look like flags.
		 */
		if (!strcmp(cmd, "--help") || !strcmp(cmd, "--version"))
			please_git_do_it_for_me();

		/*
		 * Check remaining flags.
		 */
		if (!prefixcmp(cmd, "--exec-path")) {
			please_git_do_it_for_me();
		} else if (!strcmp(cmd, "--html-path")) {
			please_git_do_it_for_me();
		} else if (!strcmp(cmd, "--man-path")) {
			please_git_do_it_for_me();
		} else if (!strcmp(cmd, "--info-path")) {
			please_git_do_it_for_me();
		} else if (!strcmp(cmd, "-p") || !strcmp(cmd, "--paginate")) {
			please_git_do_it_for_me();
		} else if (!strcmp(cmd, "--no-pager")) {
			please_git_do_it_for_me();
		} else if (!strcmp(cmd, "--no-replace-objects")) {
			please_git_do_it_for_me();
		} else if (!strcmp(cmd, "--git-dir")) {
			if (*argc < 2) {
				fprintf(stderr, "No directory given for --git-dir.\n" );
				usage(git_usage_string);
			}
			setenv(GIT_DIR_ENVIRONMENT, (*argv)[1], 1);
			(*argv)++;
			(*argc)--;
			handled++;
		} else if (!prefixcmp(cmd, "--git-dir=")) {
			setenv(GIT_DIR_ENVIRONMENT, cmd + 10, 1);
		} else if (!strcmp(cmd, "--work-tree")) {
			if (*argc < 2) {
				fprintf(stderr, "No directory given for --work-tree.\n" );
				usage(git_usage_string);
			}
			setenv(GIT_WORK_TREE_ENVIRONMENT, (*argv)[1], 1);
			(*argv)++;
			(*argc)--;
		} else if (!prefixcmp(cmd, "--work-tree=")) {
			setenv(GIT_WORK_TREE_ENVIRONMENT, cmd + 12, 1);
		} else if (!strcmp(cmd, "--bare")) {
			please_git_do_it_for_me();
		} else if (!strcmp(cmd, "-c")) {
			please_git_do_it_for_me();
		} else {
			fprintf(stderr, "Unknown option: %s\n", cmd);
			usage(git_usage_string);
		}

		(*argv)++;
		(*argc)--;
		handled++;
	}
	return handled;
}

int main(int argc, const char **argv){
	git_extract_argv0_path(argv[0]);
	git_support_register_arguments(argc, argv);
	//register argument so that we can fallback to git
	//if we can't achieve the job

	argc--;
	argv++;
	handle_options(&argv, &argc);

	if (argc == 0) {
		usage(git_usage_string);
	}

	cmd_handler handler = lookup_handler(argv[0]);
	if (handler == NULL) {
		please_git_do_it_for_me();
	}

	int code = handler(argc, argv);

	free_global_resources();

	return code;
}
