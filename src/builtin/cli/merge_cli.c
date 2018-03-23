/**
 * Automatically generated file, please don't edit!
 */
#include <stdio.h>
#include <string.h>

struct cli
{
	char *commit;
	int ff_only;
	int help;
	int merge;
	int no_commit;
	int no_ff;
};

struct cli_aux
{
	int ff_only_cmd;
	int help_cmd;
	int merge_pos;
	int no_commit_cmd;
	int no_ff_cmd;
	char *positional0;
};

typedef enum
{
	POF_VALIDATE = 1,
	POF_USAGE = 2,
} parse_cli_options_t;

static int validate_cli(struct cli *cli, struct cli_aux *aux)
{
	if (cli->help)
	{
		return 1;
	}
	if (aux->no_commit_cmd != 0 && aux->no_commit_cmd != 2)
	{
		fprintf(stderr, "Option --no-commit may be given only for the \"merge\" command\n");
		return 0;
	}
	if (aux->ff_only_cmd != 0 && aux->ff_only_cmd != 2)
	{
		fprintf(stderr, "Option --ff-only may be given only for the \"merge\" command\n");
		return 0;
	}
	if (aux->no_ff_cmd != 0 && aux->no_ff_cmd != 2)
	{
		fprintf(stderr, "Option --no-ff may be given only for the \"merge\" command\n");
		return 0;
	}
	if (cli->merge)
	{
		cli->commit = aux->positional0;
		if (!cli->commit)
		{
			fprintf(stderr, "Required argument \"commit\" is missing. Use --help for usage\n");
			return 0;
		}
	}
	else
	{
		fprintf(stderr, "Please specify a proper command. Use --help for usage.\n");
		return 0;
	}
	return 1;
}

/**
 *
 * Print usage for the given cli.
 *
 * @return 1 if usage has been printed, 0 otherwise.
 *
 */
static int usage_cli(char *cmd, struct cli *cli)
{
	if (!cli->help)
	{
		return 0;
	}
	fprintf(stderr, "usage: %s <command> [<options>]\n", cmd);
	fprintf(stderr, "merge [--no-commit] [--ff-only] [--no-ff] <commit>\n");
	return 1;
}

static int parse_cli_simple(int argc, char **argv, struct cli *cli, struct cli_aux *aux)
{
	int i;
	int cur_position = 0;
	int cur_command = -1;
	for (i=0; i < argc; i++)
	{
		if (!strcmp(argv[i], "--ff-only"))
		{
			cli->ff_only = 1;
			aux->ff_only_cmd = cur_command;
		}
		else if (!strcmp(argv[i], "--help"))
		{
			cli->help = 1;
			aux->help_cmd = cur_command;
		}
		else if (!strcmp(argv[i], "--no-commit"))
		{
			cli->no_commit = 1;
			aux->no_commit_cmd = cur_command;
		}
		else if (!strcmp(argv[i], "--no-ff"))
		{
			cli->no_ff = 1;
			aux->no_ff_cmd = cur_command;
		}
		else if (!strcmp(argv[i], "merge"))
		{
			cli->merge = 1;
			aux->merge_pos = i;
			cur_command = 2;
		}
		else if (cur_position == 0 && cur_command == 2)
		{
			aux->positional0 = argv[i];
			cur_position++;
		}
		else
		{
			fprintf(stderr, "Unknown command or option \"%s\"\n", argv[i]);
			return 0;
		}
	}
	return 1;
}

/**
 *
 * Parse the given arguments and fill the struct cli accordingly.
 *
 * @param argc as in main()
 * @param argv as in main()
 * @param cli the filled struct
 * @param opts some options to modify the behaviour of the function.
 * @return 1 if parsing was successful, 0 otherwise.
 *
 */
static int parse_cli(int argc, char **argv, struct cli *cli, parse_cli_options_t opts)
{
	struct cli_aux aux = {0};
	char *cmd = argv[0];
	if (!parse_cli_simple(argc, argv, cli, &aux))
	{
		return 0;
	}
	if (opts & POF_VALIDATE)
	{
		if (!validate_cli(cli, &aux))
		{
			return 0;
		}
	}
	if (opts & POF_USAGE)
	{
		return !usage_cli(cmd, cli);
	}
	return 1;
}

