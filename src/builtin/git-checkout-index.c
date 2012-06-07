#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "errors.h"
#include <git2.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "git-support.h"
#include "repository.h"
#include "utils.h"
#include "git-parse-mode.h"
#include "strbuf.h"
#include "fileops.h"

enum ci_type {
	CI_NON_EXIST,
	CI_DIR,
	CI_FILE,
	CI_SYMLINK,
	CI_ERROR
};

/* return the type of objpath */
enum ci_type type_of_obj(const char *objpath) {
	struct stat bufstat;
	if (lstat(objpath, &bufstat) != 0) {
		/* file not exist*/
		if (errno == ENOENT || errno == ENOTDIR) {
			return CI_NON_EXIST;
		} else {
			return CI_ERROR;
		}
	}

	if (S_ISDIR(bufstat.st_mode)) {
		return CI_DIR;
	} else if (S_ISREG(bufstat.st_mode)) {
		return CI_FILE;
	} else if(S_ISLNK(bufstat.st_mode)) {
		return CI_SYMLINK;
	} else {
		printf("bufstat.st_mode %i\n", bufstat.st_mode);
		return CI_ERROR;
	}
}

void create_force_dir(const char *objpath) {

	switch (type_of_obj(objpath)) {
		case CI_NON_EXIST: {
			/* force the parent dir to exist */
			int error = GIT_SUCCESS;
			char target_folder_path[GIT_PATH_MAX];
			error = dirname_r(target_folder_path, sizeof(target_folder_path), objpath);
			if (error < GIT_SUCCESS) {
				printf("Failed to determine parent path of %s\n", objpath);
				return;
			}
			create_force_dir(target_folder_path);
		}
		break;
		case CI_DIR:
			//nothing to do
			return;
		case CI_FILE:
		case CI_SYMLINK:
			//rm
			if (unlink(objpath) != 0) {
				printf("failed to remove %s\n", objpath);
				return;
			}
			break;
		case CI_ERROR:
			printf("Don't know how to manage file %s\n", objpath);
			return;
	}

	//create directory
	if( mkdir(objpath, 0755) != 0)
		printf("Failed to create '%s' directory\n", objpath);
}

void create_force_file(const char *objpath, int mode, const void * data, size_t data_size) {

	int p = -1;

	/* prepare */
	switch (type_of_obj(objpath)) {
		case CI_NON_EXIST: {
			/* force parent dir to exist */
			int error = GIT_SUCCESS;
			char target_folder_path[GIT_PATH_MAX];
			error = dirname_r(target_folder_path, sizeof(target_folder_path), objpath);
			if (error < GIT_SUCCESS) {
				printf("Failed to determine parent directory of %s.\n", objpath);
				return;
			}
			create_force_dir(target_folder_path);
			}
			break;
		case CI_DIR: {
			//rm -Rf objpath
			struct strbuf pathbuf = STRBUF_INIT;
			char * buf_path = xmalloc(strlen(objpath)*sizeof(char));
			memcpy(buf_path, objpath, strlen(objpath)*sizeof(char));
			strbuf_attach(&pathbuf, (void *)buf_path, strlen(objpath)*sizeof(char), strlen(objpath)*sizeof(char));
			if (remove_dir_recursively(&pathbuf) != 0) {
				printf("Failed to remove '%s'\n", objpath);
				return;
			}
			}
			break;
		case CI_FILE:
			break;
		case CI_SYMLINK:
			//rm link
			if (unlink(objpath) != 0) {
				printf("Failed to remove %s\n", objpath);
				return;
			}
			break;
		case CI_ERROR:
			printf("Don't know how to manage file %s\n", objpath);
			return;
	}

	/* write file */
	p = open(objpath, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, mode);
	
	if (p < 0) {
		printf("Error in opening file '%s'.\n", objpath);
		return;
	}
	
	write(p, data, data_size);
	close(p);
}

void create_force_symlinks(const char *objpath, const char * dest) {

	/* prepare file */
	switch (type_of_obj(objpath)) {
		case CI_NON_EXIST: {
			int error = GIT_SUCCESS;
			char target_folder_path[GIT_PATH_MAX];
			error = dirname_r(target_folder_path, sizeof(target_folder_path), objpath);
			if (error < GIT_SUCCESS) {
				printf("Failed to remove '%s'\n", objpath);
				return;
			}
			create_force_dir(target_folder_path);
		}
		break;
		case CI_DIR: {
			//rm dir
			struct strbuf pathbuf = STRBUF_INIT;
			char * buf_path = xmalloc(strlen(objpath)*sizeof(char));
			memcpy(buf_path, objpath, strlen(objpath)*sizeof(char));
			strbuf_attach(&pathbuf, (void *)buf_path, strlen(objpath)*sizeof(char), strlen(objpath)*sizeof(char));
			if (remove_dir_recursively(&pathbuf) != 0) {
				printf("Failed to remove '%s'\n", objpath);
				return;
			}
		}
		break;
		case CI_FILE:
		case CI_SYMLINK:
			//rm
			if (unlink(objpath) != 0) {
				printf("Failed to remove '%s'\n", objpath);
				return;
			}
			break;
		case CI_ERROR:
			printf("Don't know how to manage file %s\n", objpath);
			return;
	}

	if(symlink(dest ,objpath)!= 0)
		printf("Failed to create symlink '%s'\n", objpath);
}



int cmd_checkout_index(int argc, const char **argv)
{
	/* options parsing */
	if (argc != 3)
		please_git_do_it_for_me();

	
	if (!(strcmp(argv[1], "-f") == 0 && strcmp(argv[2], "-a") == 0))
		please_git_do_it_for_me();


	git_repository *repo = get_git_repository();
	
	git_index *index_cur;
	int e = git_repository_index(&index_cur, repo);
	if (e) libgit_error();

	
	char *buf = (char*)xmalloc(GIT_OID_HEXSZ+1);
	
// 	const char *prefix = get_git_prefix();
// 	size_t prefix_len = strlen(prefix);
	
	for (unsigned i = 0; i < git_index_entrycount(index_cur); i++) {
		git_index_entry *gie = git_index_get(index_cur, i);
// 		printf("%06o\t%s\n", gie->mode, gie->path);
		git_odb_object * obj;
		e = git_odb_read(&obj, git_repository_database(repo), &gie->oid);
		if(e != GIT_SUCCESS)
			libgit_error();
		//TODO:tester le type et lancer le bon cretae_force_* en fonction...
		switch (gie->mode >>12 ) {
			case 0xA:
				create_force_symlinks(gie->path, (char *)git_odb_object_data(obj));
				break;
			case 0x8:
				create_force_file(gie->path, gie->mode, git_odb_object_data(obj), git_odb_object_size(obj));
				break;
			default:
				printf("Don't know this kind of file : %06o\t%s\n", gie->mode, gie->path);
		}
		
		
	}

	
	free(buf);
	git_index_free(index_cur);
	
	return EXIT_SUCCESS;
}
