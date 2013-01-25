#ifndef GIT_EXEC_CMD_H
#define GIT_EXEC_CMD_H

#include "common.h"

typedef struct cmd_struct{
	char *cmd;
	git_cb handler;
} cmd_struct;

#endif /* GIT_EXEC_CMD_H */
