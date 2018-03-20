/**
 * Automatically generated file, please don't edit!
 */
#include <stdio.h>
#include <string.h>

struct cli
{
	int ff_only;
	int help;
	int pull;
	char *refspec;
	char *repository;
};

struct cli_aux
{
	int ff_only_cmd;
	int help_cmd;
	char *positional0;
	char *positional1;
	int pull_pos;
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
	if (aux->ff_only_cmd != 0 && aux->ff_only_cmd != 2)
	{
		fprintf(stderr, "Option --ff-only may be given only for the \"pull\" command\n");
		return 0;
	}
	if (cli->pull)
	{
		cli->repository = aux->positional0;
		cli->refspec = aux->positional1;
		if (!cli->repository)
		{
			fprintf(stderr, "Required argument \"repository\" is missing. Use --help for usage\n");
			return 0;
		}
		if (!cli->refspec)
		{
			fprintf(stderr, "Required argument \"refspec\" is missing. Use --help for usage\n");
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
	fprintf(stderr, "pull [--ff-only] <repository> <refspec>\n");
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
		else if (!strcmp(argv[i], "pull"))
		{
			cli->pull = 1;
			aux->pull_pos = i;
			cur_command = 2;
		}
		else if (cur_position == 0 && cur_command == 2)
		{
			aux->positional0 = argv[i];
			cur_position++;
		}
		else if (cur_position == 1 && cur_command == 2)
		{
			aux->positional1 = argv[i];
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

