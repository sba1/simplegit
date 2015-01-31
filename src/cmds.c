/**
 * @file
 */
#include "cmds.h"
#include "strbuf.h"

#include "reset.h"
#include "stash.h"

struct cmd_struct {
	char *cmd;
	int plumbing;
	char *desc;
	git_cb handler;
};

struct cmd_struct commands[] = {
	{"add",            1, "Add contents of file to the index", cmd_add},
	{"branch",         1, "Create, delete, or delete branches", cmd_branch},
	{"cat-file",       0, "Show content of repository objects", cmd_cat_file},
	{"checkout-index", 0, "Copy files from the index", cmd_checkout_index},
	{"checkout",       1, "Copy a branch or paths to the working directory", cmd_checkout},
	{"clone",          1, "Fetch and checkout a repository into a new directory", do_clone},
	{"commit-tree",    0, "Create a new commit object from a tree object", cmd_commit_tree},
	{"commit",         1, "Create a new commit from the index", cmd_commit},
	{"config",         1, "Get and set repository or global options", cmd_config},
	{"diff-files",     0, "Show differences between files in the working directory and the index", cmd_diff_files},
	{"diff",           1, "Show differences between objects", cmd_diff},
	{"fetch",          1, "Fetch objects from another repository", fetch},
	{"index-pack",     0, "Build the index file for an existing pack file", index_pack},
	{"init",           1, "Create or reinitialize a git repository", cmd_init},
	{"log",            1, "Show the commit logs", cmd_log},
	{"ls-files",       0, "Show information about files in the index and the working directory", cmd_ls_files},
	{"ls-remote",      0, "Show information about remote references", ls_remote},
	{"ls-tree",        0, "Show information about a tree object", cmd_ls_tree},
	{"mktag",          0, "Create a tag object", cmd_mktag},
	{"merge",          1, "Join histories defined by commit together", cmd_merge},
	{"push",           1, "Update the remote references and push associated objects", cmd_push},
	{"read-tree",      0, "Read a tree object into the index", cmd_read_tree},
	{"reset",          1, "Reset the HEAD or the index to a specific state", cmd_reset},
	{"rev-list",       0, "Show information about commit objects", cmd_rev_list},
	{"show",           1, "Show objects", cmd_show},
	{"stash",          1, "Manage separate the separate stash of changes", cmd_stash},
	{"status",         1, "Summarize working directory changes", cmd_status},
	{"update-index",   0, "Update the index with files from the working directory", cmd_update_index},
	{"write-tree",     0, "Create a write object from the current index", cmd_write_tree},
	{"version",        1, "Show version information.", cmd_version},
};

void print_commands_overview()
{
	size_t i;
	size_t max = sizeof(commands) / sizeof(commands[0]);

	fprintf(stderr, "Following plumbing commands are supported:\n");
	for (i = 0; i < max; i++)
	{
		if (commands[i].plumbing)
			fprintf(stderr, "  %-15s %s\n", commands[i].cmd, commands[i].desc);
	}
	fprintf(stderr, "Following porcelaine commands are supported:\n");
	for (i = 0; i < max; i++)
	{
		if (!commands[i].plumbing)
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
