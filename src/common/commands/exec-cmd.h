#ifndef EXEC_CMD_H
#define EXEC_CMD_H

// original source : https://github.com/vfr-nl/git2

typedef int (*cmd_handler)(int, const char**);

typedef struct cmd_struct{
	char *cmd;
		cmd_handler handler;
} cmd_struct;

void cmd_set_argv_exec_path(const char *exec_path);
const char *cmd_extract_argv0_path(const char *path);
const char *cmd_exec_path(void);
void setup_path(void);
const char **prepare_git_cmd(const char **argv);
int execv_git_cmd(const char **argv); /* NULL terminated */
int execl_git_cmd(const char *cmd, ...);
const char *system_path(const char *path);

#endif /* EXEC_CMD_H */
