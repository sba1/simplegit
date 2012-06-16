#include "errors.h"
#include <git2.h>
#include "common.h"

int clone(int argc, char **argv)
{   
	// Let's suppose the URL looks like: https://github.com/libgit2/libgit2.git
	// Then we need to get a URL like this too: git://github.com/libgit2/libgit2.git
	// This may be a bit dodgy, but it will do for now.
	//const char *gitURL = [remoteURL.absoluteString stringByReplacingOccurrencesOfString:@"https://github.com/" withString:@"git://github.com/"].UTF8String;

	//Setup
	int error;
	git_repository *repo;
	git_config *cfg;
	git_remote *remote;

	error = git_repository_open(&repo, ".git");
	
	if (error != GIT_SUCCESS) {
		git_repository_init(&repo, ".git", 1);

		//Config
		git_repository_config(&cfg, repo);
		git_config_set_int32 (cfg, "core.repositoryformatversion", 0);
		git_config_set_bool (cfg, "core.filemode", 1);
		git_config_set_bool (cfg, "core.bare", 0);
		git_config_set_bool (cfg, "core.logallrefupdates", 1);
		git_config_set_bool (cfg, "core.ignorecase", 1);
		git_config_set_string (cfg, "remote.origin.url", argv[1]);
		git_config_set_string (cfg, "remote.origin.fetch", "+refs/heads/*:refs/remotes/origin/*");
		git_config_set_string (cfg, "branch.master.remote", "origin");
		git_config_set_string (cfg, "branch.master.merge", "refs/heads/master");

		git_repository_set_workdir(repo, ".git");

		error = git_remote_new(&remote, repo, "A remote", argv[1], "origin");

		git_repository_free(repo);
		git_repository_open (&repo, ".git");
	}

	git_repository_config(&cfg, repo);

	// connect to repo
	error = git_remote_load(&remote, repo, "origin");

	error = git_remote_connect(remote, GIT_DIR_FETCH);
	// get pack file

	git_off_t bytes;
	git_indexer_stats stats;
	error = git_remote_download(remote, &bytes, &stats);

	char *packFolderURL = ".git/objects/pack";
	char *packname = NULL;
	DIR *dir = NULL;
	struct dirent *de;

	if ((dir = opendir(packFolderURL)) == NULL) {
		giterr_set(GITERR_OS, "Failed to open directory '%s'", packFolderURL);
		return -1;
	}

	while(de = readdir(dir))
	{
		if(strstr(de->d_name, ".pack") == NULL) continue;
		
		asprintf(&packname, "%s/%s", packFolderURL, de->d_name);
		printf("packfile: %s\n", packname);
		
		// unpack pack file
		if (packname != NULL)
		{
			git_indexer *indexer;
			git_indexer_stats stats2;
			int error;
			char hash[GIT_OID_HEXSZ + 1] = {0};
			char *indexPath = NULL;
	
			error = git_indexer_new(&indexer, packname);
			error = git_indexer_run(indexer, &stats2);
			error = git_indexer_write(indexer);

			// Get the packfile's hash (which should become it's filename)
			git_oid_fmt(hash, git_indexer_hash(indexer));

			asprintf(&indexPath, "%s/pack-%s.idx", packFolderURL, hash);

			puts(hash);
			git_index *index;
			git_index_open(&index, indexPath);
			git_index_read(index);
			git_repository_set_index(repo, index);

			git_indexer_free(indexer);
			git_remote_update_tips(remote, update_cb); //No idea what it does, but it seems like it's important… It does make the branches appear in .git/refs/remotes/origin

			free(indexPath);
			free(packname);
		}
	}
	
	if(dir) closedir(dir);
	
//Somehow do git checkout master here

return 0;

}
