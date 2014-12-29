#include "builtin.h"
#include "strbuf.h"

struct cmd_struct {
	char *cmd;
	git_cb handler;
};

struct cmd_struct commands[] = {
	{"init", cmd_init},
	{"rev-list", cmd_rev_list},
	{"ls-files", cmd_ls_files},
	{"checkout", cmd_checkout},
	{"ls-tree", cmd_ls_tree},
	{"update-index", cmd_update_index},
	{"mktag", cmd_mktag},
	{"commit-tree", cmd_commit_tree},
	{"write-tree", cmd_write_tree},
	{"read-tree", cmd_read_tree},
	{"checkout-index", cmd_checkout_index},
	{"cat-file", cmd_cat_file},
	{"ls-remote", ls_remote},
	{"fetch", fetch},
	{"index-pack", index_pack},
	{"clone", do_clone},
	{"branch", cmd_branch},
	{"log", cmd_log},
	{"add", cmd_add},
	{"diff-files", cmd_diff_files},
	{"show", cmd_show},
	{"config", cmd_config},
	{"commit", cmd_commit},
	{"push", cmd_push},
	{"diff", cmd_diff},
	{"status", cmd_status},
};

git_cb lookup_handler(const char *cmd) {
	unsigned int i;
	unsigned int max = sizeof(commands) / sizeof(commands[0]);

	for (i = 0; i < max; i++)
		if (!strcmp(commands[i].cmd, cmd))
			return commands[i].handler;
	return NULL;
}
