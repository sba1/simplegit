#ifndef GIT_CAT_FILE_H
#define GIT_CAT_FILE_H

#include <git2.h>

int cmd_cat_file(struct git_repository *repo, int argc, char **argv);

#endif
