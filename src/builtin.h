#ifndef BUILTIN_H
#define BUILTIN_H

// original source : https://github.com/vfr-nl/git2

#include "exec-cmd.h"
#include "git-init.h"
#include "git-log.h"
#include "git-ls-files.h"
#include "git-checkout.h"
#include "git-ls-tree.h"
#include "git-update-index.h"
#include "git-mktag.h"
#include "git-commit-tree.h"

int cmd_init(int argc, const char **argv);

cmd_struct commands[] = {
	{"init", cmd_init},
	{"log", cmd_log},
	{"ls-files", cmd_ls_files},
	{"checkout", cmd_checkout},
	{"ls-tree", cmd_ls_tree},
	{"update-index", cmd_update_index},
	{"mktag",cmd_mktag},
	{"commit-tree",cmd_commit_tree}
};
extern cmd_struct commands[];

cmd_handler lookup_handler(const char *cmd);

#endif /* BUILTIN_H */
