/**
 * Automatically generated file, please don't edit!
 */
#include <stdio.h>
#include <string.h>

struct cli
{
	int add;
	int help;
	char **pathspec;
	int pathspec_count;
};

struct cli_aux
{
	int add_pos;
	int help_cmd;
	int variadic_argc;
	char **variadic_argv;
};

typedef enum
{
	POF_VALIDATE = (1<<0),
	POF_USAGE = (1<<1)
} parse_cli_options_t;

static int validate_cli(struct cli *cli, struct cli_aux *aux)
{
	if (cli->help)
	{
		return 1;
	}
	if (cli->add)
	{
		cli->pathspec_count = aux->variadic_argc;
		cli->pathspec = aux->variadic_argv;
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
	fprintf(stderr, "add [<pathspec>...]\n");
	return 1;
}

static int parse_cli_simple(int argc, char **argv, struct cli *cli, struct cli_aux *aux)
{
	int i;
	int cur_position = 0;
	int cur_command = -1;
	for (i=0; i < argc; i++)
	{
		if (!strcmp("--help", argv[i]))
		{
			cli->help = 1;
			aux->help_cmd = cur_command;
		}
		else if (!strcmp("add", argv[i]))
		{
			cli->add = 1;
			aux->add_pos = i;
			cur_command = 2;
		}
		else if (cur_position == 0 && cur_command == 2)
		{
			aux->variadic_argv = &argv[i];
			aux->variadic_argc = (argc) - (i);
			break;
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
	argc--;
	argv++;
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

