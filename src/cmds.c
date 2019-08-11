/**
 * @file
 */

#include "cmds.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "builtin/common.h"

#include "builtin/add.h"
#include "builtin/branch.h"
#include "builtin/cat-file.h"
#include "builtin/checkout-index.h"
#include "builtin/checkout.h"
#include "builtin/cherry-pick.h"
#include "builtin/clone.h"
#include "builtin/commit-tree.h"
#include "builtin/commit.h"
#include "builtin/config.h"
#include "builtin/diff-files.h"
#include "builtin/diff.h"
#include "builtin/fetch.h"
#include "builtin/help.h"
#include "builtin/index-pack.h"
#include "builtin/init.h"
#include "builtin/log.h"
#include "builtin/ls-files.h"
#include "builtin/ls-remote.h"
#include "builtin/ls-tree.h"
#include "builtin/merge.h"
#include "builtin/mktag.h"
#include "builtin/pull.h"
#include "builtin/push.h"
#include "builtin/read-tree.h"
#include "builtin/rebase.h"
#include "builtin/remote.h"
#include "builtin/reset.h"
#include "builtin/rev-list.h"
#include "builtin/rm.h"
#include "builtin/show.h"
#include "builtin/stash.h"
#include "builtin/status.h"
#include "builtin/submodule.h"
#include "builtin/tag.h"
#include "builtin/update-index.h"
#include "builtin/version.h"
#include "builtin/write-tree.h"

struct cmd_struct {
	const char *cmd;
	int plumbing;
	const char *desc;
	git_cb handler;
};

static const struct cmd_struct commands[] = {
	{"add",            1, "Add contents of file to the index", cmd_add},
	{"branch",         1, "Create, delete, or delete branches", cmd_branch},
	{"cat-file",       0, "Show content of repository objects", cmd_cat_file},
	{"checkout-index", 0, "Copy files from the index", cmd_checkout_index},
	{"checkout",       1, "Copy a branch or paths to the working directory", cmd_checkout},
	{"cherry-pick",    1, "Redo commits", cmd_cherry_pick},
	{"clone",          1, "Fetch and checkout a repository into a new directory", do_clone},
	{"commit-tree",    0, "Create a new commit object from a tree object", cmd_commit_tree},
	{"commit",         1, "Create a new commit from the index", cmd_commit},
	{"config",         1, "Get and set repository or global options", cmd_config},
	{"diff-files",     0, "Show differences between files in the working directory and the index", cmd_diff_files},
	{"diff",           1, "Show differences between objects", cmd_diff},
	{"fetch",          1, "Fetch objects from another repository", fetch},
	{"help",           0, "Show this help", cmd_help},
	{"index-pack",     0, "Build the index file for an existing pack file", index_pack},
	{"init",           1, "Create or reinitialize a git repository", cmd_init},
	{"log",            1, "Show the commit logs", cmd_log},
	{"ls-files",       0, "Show information about files in the index and the working directory", cmd_ls_files},
	{"ls-remote",      0, "Show information about remote references", ls_remote},
	{"ls-tree",        0, "Show information about a tree object", cmd_ls_tree},
	{"mktag",          0, "Create a tag object", cmd_mktag},
	{"merge",          1, "Join histories defined by commit together", cmd_merge},
	{"pull",           1, "Fetch and merge changes from branches of remote repositories", cmd_pull},
	{"push",           1, "Update the remote references and push associated objects", cmd_push},
	{"read-tree",      0, "Read a tree object into the index", cmd_read_tree},
	{"rebase",         1, "Reapply changes", cmd_rebase},
	{"remote",         1, "Manage remotes", cmd_remote},
	{"reset",          1, "Reset the HEAD or the index to a specific state", cmd_reset},
	{"rev-list",       0, "Show information about commit objects", cmd_rev_list},
	{"rm",             1, "Remove files", cmd_rm},
	{"show",           1, "Show objects", cmd_show},
	{"stash",          1, "Manage a separate stash of changes", cmd_stash},
	{"status",         1, "Summarize working directory changes", cmd_status},
	{"submodule",      1, "Manage submodules", cmd_submodule},
	{"tag",            1, "Manage tags", cmd_tag},
	{"update-index",   0, "Update the index with files from the working directory", cmd_update_index},
	{"write-tree",     0, "Create a write object from the current index", cmd_write_tree},
	{"version",        1, "Show version information", cmd_version},
};

void print_commands_overview()
{
	size_t i;
	size_t max = sizeof(commands) / sizeof(commands[0]);

	fprintf(stdout, "Following plumbing commands are supported:\n");
	for (i = 0; i < max; i++)
	{
		if (commands[i].plumbing)
			fprintf(stdout, "  %-15s %s\n", commands[i].cmd, commands[i].desc);
	}
	fprintf(stdout, "Following porcelaine commands are supported:\n");
	for (i = 0; i < max; i++)
	{
		if (!commands[i].plumbing)
			fprintf(stdout, "  %-15s %s\n", commands[i].cmd, commands[i].desc);
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
