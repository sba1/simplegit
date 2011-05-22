#ifndef BUILTIN_H
#define BUILTIN_H

// original source : https://github.com/vfr-nl/git2

#include "exec-cmd.h"
#include "git-init.h"
#include "git-log.h"

int cmd_init(int argc, const char **argv);

cmd_struct commands[] = {
	{"init", cmd_init},
	{"log", cmd_log}
};

#endif /* BUILTIN_H */
