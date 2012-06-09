#ifndef BUILTIN_H
#define BUILTIN_H

// original source : https://github.com/vfr-nl/git2

#include "exec-cmd.h"
#include "git-init.h"
#include "git-rev-list.h"
#include "git-ls-files.h"
#include "git-checkout.h"
#include "git-ls-tree.h"
#include "git-update-index.h"
#include "git-mktag.h"
#include "git-commit-tree.h"
#include "git-write-tree.h"
#include "git-read-tree.h"
#include "git-checkout-index.h"
#include "git-cat-file.h"

#include "common.h"

int cmd_init(int argc, const char **argv);

extern cmd_struct commands[];

cmd_handler lookup_handler(const char *cmd);

#endif /* BUILTIN_H */
