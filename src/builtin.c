#include "builtin.h"
#include "strbuf.h"

cmd_struct commands[] = {
	{"init", cmd_init},
	{"log", cmd_log},
	{"ls-files", cmd_ls_files},
	{"checkout", cmd_checkout},
	{"ls-tree", cmd_ls_tree},
	{"update-index", cmd_update_index},
	{"mktag", cmd_mktag},
	{"commit-tree", cmd_commit_tree},
	{"write-tree", cmd_write_tree},
	{"read-tree", cmd_read_tree},
	{"checkout-index", cmd_checkout_index},
	{"cat_file", cmd_cat_file}
};

cmd_handler lookup_handler(const char *cmd) {
	unsigned int i;
	unsigned int max = sizeof(commands) / sizeof(cmd_struct);

	for (i = 0; i < max; i++)
		if (!strcmp(commands[i].cmd, cmd))
			return commands[i].handler;
	return NULL;
}
