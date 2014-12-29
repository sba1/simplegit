#ifndef BUILTIN_H
#define BUILTIN_H

// original source : https://github.com/vfr-nl/git2

#include <git2.h>

#include "git-add.h"
#include "git-diff-files.h"
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
#include "git-branch.h"
#include "git-log.h"
#include "git-show.h"
#include "git-config.h"
#include "git-commit.h"
#include "git-push.h"
#include "git-diff.h"
#include "git-status.h"

#include "common.h"

/**
 * Prints an overview of all supported commands.
 */
void print_commands_overview(void);

git_cb lookup_handler(const char *cmd);

#endif /* BUILTIN_H */
