#ifndef GIT_LOG_H
#define GIT_LOG_H

int cmd_log(git_repository *repo, int argc, char **argv);

void print_commit(git_commit *wcommit, int with_tree);

#endif
