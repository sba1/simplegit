#ifndef CMDS_H
#define CMDS_H

#include "common.h"

/**
 * Prints an overview of all supported commands.
 */
void print_commands_overview(void);

git_cb lookup_handler(const char *cmd);

#endif /* CMDS_H */
