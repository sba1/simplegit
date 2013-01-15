#ifndef GIT_COMMIT_TREE_H
#define GIT_COMMIT_TREE_H

int cmd_commit_tree(git_repository *repo, int argc, char **argv);

int sgit_get_author_signature(git_signature **author_signature);
int sgit_get_committer_signature(git_signature** committer_signature);

#endif

