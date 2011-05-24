#include "index-entry.h"

int get_stage(git_index_entry *entry) {
	return ((INDEX_ENTRY_STAGEMASK & entry->flags) >> INDEX_ENTRY_STAGESHIFT);
}
