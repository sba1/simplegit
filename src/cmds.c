/**
 * @file
 */
#include "cmds.h"
#include "strbuf.h"

#include "reset.h"

struct cmd_struct {
	char *cmd;
	char *desc;
	git_cb handler;
};

struct cmd_struct commands[] = {
	{"add",            "", cmd_add},
	{"branch",         "", cmd_branch},
	{"cat-file",       "", cmd_cat_file},
	{"checkout-index", "", cmd_checkout_index},
	{"checkout",       "", cmd_checkout},
	{"clone",          "Clone a repository", do_clone},
	{"commit-tree",    "", cmd_commit_tree},
	{"commit",         "", cmd_commit},
	{"config",         "", cmd_config},
	{"diff-files",     "", cmd_diff_files},
	{"diff",           "Show changes between objects", cmd_diff},
	{"fetch",          "Fetch objects from another repository", fetch},
	{"index-pack",     "", index_pack},
	{"init",           "Create or reinitialize a git repository", cmd_init},
	{"log",            "", cmd_log},
	{"ls-files",       "", cmd_ls_files},
	{"ls-remote",      "", ls_remote},
	{"ls-tree",        "", cmd_ls_tree},
	{"mktag",          "", cmd_mktag},
	{"push",           "", cmd_push},
	{"read-tree",      "", cmd_read_tree},
	{"reset",          "", cmd_reset},
	{"rev-list",       "", cmd_rev_list},
	{"show",           "", cmd_show},
	{"status",         "Summarize working directory changes", cmd_status},
	{"update-index",   "", cmd_update_index},
	{"write-tree",     "", cmd_write_tree},
};

void print_commands_overview()
{
	size_t i;
	size_t max = sizeof(commands) / sizeof(commands[0]);

	fprintf(stderr, "Following commands are supported:\n");
	for (i = 0; i < max; i++)
	{
		fprintf(stderr, "  %-15s %s\n", commands[i].cmd, commands[i].desc);
	}
}

git_cb lookup_handler(const char *cmd) {
	unsigned int i;
	unsigned int max = sizeof(commands) / sizeof(commands[0]);

	for (i = 0; i < max; i++)
		if (!strcmp(commands[i].cmd, cmd))
			return commands[i].handler;
	return NULL;
}
