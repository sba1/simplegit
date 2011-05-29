#include <string.h>
#include "git.h"
#include "builtin.h"
#include "exec-cmd.h"
#include "git-support.h"
#include "run-command.h"
#include "errors.h"
#include "parse-options.h"
#include "repository.h"

// original source : https://github.com/vfr-nl/git2
#define NO_INFO 0
#define INFO_VERSION 1
#define INFO_HELP 2
#define INFO_EXEC_PATH 3
#define INFO_HTML_PATH 4
#define INFO_MAN_PATH 5
#define INFO_INFO_PATH 6

const char * const git_usage_string[] = {
	"git [--version] [--exec-path[=<path>]] [--html-path] [--man-path] [--info-path]\n"
	"           [-p|--paginate|--no-pager] [--no-replace-objects]\n"
	"           [--bare] [--git-dir=<path>] [--work-tree=<path>]\n"
	"           [-c name=value] [--help]\n"
	"           <command> [<args>]",
	NULL
};

cmd_handler lookup_handler(const char *cmd){
	unsigned int i;
	unsigned int max = sizeof(commands) / sizeof(cmd_struct);

	for (i = 0; i < max; i++)
		if (!strcmp(commands[i].cmd, cmd))
			return commands[i].handler;
	return NULL;
}

void free_global_resources() {
	git_support_free_arguments();
	git_exec_cmd_free_resources();
	free_repository();
}

static int version_callback(const struct option *opt, const char *arg, int unset) {
	(void)opt;
	(void)arg;
	(void)unset;

	int* info_value = (int*)(opt->value);

	if (*info_value == NO_INFO) {
		*info_value = INFO_VERSION;
	}

	return 0;
}

static int help_callback(const struct option *opt, const char *arg, int unset) {
	(void)opt;
	(void)arg;
	(void)unset;

	int* info_value = (int*)(opt->value);

	if (*info_value == NO_INFO) {
		*info_value = INFO_HELP;
	}

	return 0;
}

static int exec_path_callback(const struct option *opt, const char *arg, int unset) {
	(void)opt;
	(void)unset;

	if (arg) {
		
	}

	int* info_value = (int*)(opt->value);

	if (*info_value == NO_INFO) {
		*info_value = INFO_EXEC_PATH;
	}

	return 0;
}

static int html_path_callback(const struct option *opt, const char *arg, int unset) {
	(void)opt;
	(void)arg;
	(void)unset;

	int* info_value = (int*)(opt->value);

	if (*info_value == NO_INFO) {
		*info_value = INFO_HTML_PATH;
	}

	return 0;
}

static int man_path_callback(const struct option *opt, const char *arg, int unset) {
	(void)opt;
	(void)arg;
	(void)unset;

	int* info_value = (int*)(opt->value);

	if (*info_value == NO_INFO) {
		*info_value = INFO_MAN_PATH;
	}

	return 0;
}

static int info_path_callback(const struct option *opt, const char *arg, int unset) {
	(void)opt;
	(void)arg;
	(void)unset;

	int* info_value = (int*)(opt->value);

	if (*info_value == NO_INFO) {
		*info_value = INFO_INFO_PATH;
	}

	return 0;
}

static int configuration_callback(const struct option *opt, const char *arg, int unset) {
	(void)opt;
	(void)arg;
	(void)unset;

	*((int*)(opt->value)) = 1;

	return 0;
}

int main(int argc, const char **argv){
	git_extract_argv0_path(argv[0]);
	git_support_register_arguments(argc, argv);
	//register argument so that we can fallback to git
	//if we can't achieve the job

	int offset;
	for (offset = 1; offset < argc; ++offset) {
		if (strcmp(argv[offset], "-c") == 0) {
			if (offset + 1 < argc) {
				++offset;
			}
		} else if (argv[offset][0] != '-') {
			break;
		}
	}

	int info_output = NO_INFO;
	unsigned int pager_flag = 1;
	unsigned int no_replace_object_flag = 0;
	unsigned int bare_flag = 0;
	unsigned int configuration_is_set = 0;
	char exec_path[PATH_MAX] = "";
	char git_dir[PATH_MAX] = "";
	char work_tree[PATH_MAX] = "";

	const struct option options[] = {
		{OPTION_CALLBACK, '\0', "version", &info_output, NULL, "Print git version", PARSE_OPT_NOARG, version_callback, 0},
		{OPTION_CALLBACK, '\0', "help", &info_output, NULL, "Print git help", PARSE_OPT_NOARG, help_callback, 0},
		{OPTION_CALLBACK, '\0', "exec-path", &info_output, NULL, "Print git exec path", PARSE_OPT_OPTARG, exec_path_callback, 0},
		{OPTION_CALLBACK, '\0', "html-path", &info_output, NULL, "Print git html path", PARSE_OPT_NOARG, html_path_callback, 0},
		{OPTION_CALLBACK, '\0', "man-path", &info_output, NULL, "Print git man path", PARSE_OPT_NOARG, man_path_callback, 0},
		{OPTION_CALLBACK, '\0', "info-path", &info_output, NULL, "Print git info path", PARSE_OPT_NOARG, info_path_callback, 0},
		OPT_BIT('p', "paginate", &pager_flag, "Pipe output in the terminal pager", 1),
		OPT_BIT('\0', "no-pager", &pager_flag, "Disable outputing in the terminal pager", 0),
		OPT_BIT('\0', "bare", &bare_flag, "Treat repository as a bare one", 1),
		OPT_BIT('\0', "bare", &no_replace_object_flag, "Do not use replacement refs to replace git objects.", 1),
		OPT_STRING('\0',"git-dir", &git_dir, "path", "Change git dir"),
		OPT_STRING('\0',"work-tree", &work_tree, "path", "Change work tree"),
		{OPTION_CALLBACK, 'c', NULL, &configuration_is_set, "name=value", "Change a configuration entry", 0, configuration_callback, 0},
		OPT_END()
	};

	char prefix[] = "";

	parse_options(offset, argv, prefix, options, git_usage_string, 0);

	//pager_flag is true by default, but we don't handle for now
	if (info_output != NO_INFO ||  no_replace_object_flag || exec_path[0] != '\0' || configuration_is_set) {
		//nothing is handled for now
		please_git_do_it_for_me();
	}
	
	if (offset == argc) {
		usage(git_usage_string[0]);
	}

	cmd_handler handler = lookup_handler(argv[offset]);
	if (handler == NULL) {
		please_git_do_it_for_me();
	}

	int code = handler(argc - offset, (const char**)(argv + offset));

	free_global_resources();

	return code;
}
