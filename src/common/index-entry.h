#ifndef INDEX_ENTRY_H
#define INDEX_ENTRY_H

#include <git2.h>

#define INDEX_ENTRY_STAGEMASK (0x3000)
#define INDEX_ENTRY_STAGESHIFT 12

int get_stage(git_index_entry *entry);

#endif
