#ifndef CMDS_H
#define CMDS_H

// original source : https://github.com/vfr-nl/git2

#include <git2.h>
#include "builtin/add.h"
#include "builtin/branch.h"
#include "builtin/cat-file.h"
#include "builtin/checkout.h"
#include "builtin/checkout-index.h"
#include "builtin/commit.h"
#include "builtin/commit-tree.h"
#include "builtin/config.h"
#include "builtin/diff.h"
#include "builtin/diff-files.h"
#include "builtin/init.h"
#include "builtin/log.h"
#include "builtin/ls-files.h"
#include "builtin/ls-tree.h"
#include "builtin/merge.h"
#include "builtin/mktag.h"
#include "builtin/push.h"
#include "builtin/read-tree.h"
#include "builtin/rev-list.h"
#include "builtin/show.h"
#include "builtin/status.h"
#include "builtin/update-index.h"
#include "builtin/write-tree.h"
#include "builtin/version.h"

#include "common.h"

/**
 * Prints an overview of all supported commands.
 */
void print_commands_overview(void);

git_cb lookup_handler(const char *cmd);

#endif /* CMDS_H */
