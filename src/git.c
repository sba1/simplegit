/**
 * @file
 *
 * This is a main entry for the simple git tool.
 */
#include "git.h"

#include "cmds.h"
#include "environment.h"
#include "errors.h"
#include "git-support.h"
#include "strbuf.h"

#include <string.h>

static char *argv0_path;

static const char *git_extract_argv0_path(const char *argv0)
{
	const char *slash;

	if (!argv0 || !*argv0)
		return NULL;
	slash = argv0 + strlen(argv0);

	while (argv0 <= slash && !is_dir_sep(*slash))
		slash--;

	if (slash >= argv0)
	{
		argv0_path = strndup(argv0, slash - argv0);
		return slash + 1;
	}

	return argv0;
}

#if defined (AMIGA)
__attribute__((unused)) static const char version_tag[] =
	"$VER: sgit 0." SIMPLEGIT_REVISION_STRING " (" SIMPLEGIT_DATE_STRING ")";
#endif

/**
 * Main entry.
 *
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv)
{
	git_repository *repo = NULL;
	const char *cmd;
	int rc = EXIT_FAILURE;
	int err;
	int git_libgit2_initialied_err;

	if ((git_libgit2_initialied_err = git_libgit2_init()))
	{
		fprintf(stderr,"Thread system initialization failed!\n");
		goto out;
	}

	cmd = git_extract_argv0_path(argv[0]);

	if (!prefixcmp(cmd,"git-"))
	{
		cmd += 4;
		argv[0] = (char*)cmd;
	} else
	{
		int i;

		argc--;
		argv++;

		for (i=0;i<argc;i++)
		{
			if (argv[0][0] != '-') break;

			if (!strcmp("--exec-path",argv[i]))
			{
				printf(argv0_path);
				rc = EXIT_SUCCESS;
				goto out;
			}
		}
	}

	if (argc <= 0)
	{
		fprintf(stderr,"Usage: %s <command>\n", cmd);
		print_commands_overview();
		goto out;
	}

	err = git_repository_open(&repo, ".git");
	if (err < 0)
	{
		/* Does the command require a repo? Check against those that do not
		 * require one */
		if (strcmp(argv[0], "init") && strcmp(argv[0], "config") && strcmp(argv[0], "clone"))
		{
			libgit_error();
			goto out;
		}

		repo = NULL;
	}

	git_cb handler = lookup_handler(argv[0]);
	if (handler != NULL)
	{
		rc = handler(repo, argc, argv);
	} else
	{
		fprintf(stderr,"Command \"%s\" not supported\n",argv[0]);
		rc = 20;
	}
out:
	git_repository_free(repo);
	if (!git_libgit2_initialied_err)
		git_libgit2_shutdown();
	return rc;
}
