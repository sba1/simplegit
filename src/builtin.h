#ifndef BUILTIN_H
#define BUILTIN_H

// original source : https://github.com/vfr-nl/git2

#include "exec-cmd.h"
#include "git-init.h"
#include "git-log.h"
#include "git-ls-files.h"
#include "git-checkout.h"

int cmd_init(int argc, const char **argv);

cmd_struct commands[] = {
	{"init", cmd_init},
	{"log", cmd_log},
	{"ls-files", cmd_ls_files},
	{"checkout", cmd_checkout}
};

#endif /* BUILTIN_H */
