#ifndef EXEC_CMD_H
#define EXEC_CMD_H

// original source : https://github.com/vfr-nl/git2

typedef int (*cmd_handler)(int, const char**);

typedef struct cmd_struct{
	char *cmd;
		cmd_handler handler;
} cmd_struct;

#endif /* EXEC_CMD_H */
