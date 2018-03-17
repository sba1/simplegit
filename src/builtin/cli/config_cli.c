/**
 * Automatically generated file, please don't edit!
 */
#include <stdio.h>
#include <string.h>

struct cli
{
	int config;
	int global;
	int help;
	char *name;
	char *value;
};

struct cli_aux
{
	int config_pos;
	int global_cmd;
	int help_cmd;
	char *positional0;
	char *positional1;
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
	if (aux->global_cmd != 0 && aux->global_cmd != 2)
	{
		fprintf(stderr, "Option --global may be given only for the \"config\" command\n");
		return 0;
	}
	if (cli->config)
	{
		cli->name = aux->positional0;
		cli->value = aux->positional1;
		if (!cli->name)
		{
			fprintf(stderr, "Required argument \"name\" is missing. Use --help for usage\n");
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
	fprintf(stderr, "config [--global] <name> [<value>]\n");
	return 1;
}

static int parse_cli_simple(int argc, char **argv, struct cli *cli, struct cli_aux *aux)
{
	int i;
	int cur_position = 0;
	int cur_command = -1;
	for (i=0; i < argc; i++)
	{
		if (!strcmp(argv[i], "--global"))
		{
			cli->global = 1;
			aux->global_cmd = cur_command;
		}
		else if (!strcmp(argv[i], "--help"))
		{
			cli->help = 1;
			aux->help_cmd = cur_command;
		}
		else if (!strcmp(argv[i], "config"))
		{
			cli->config = 1;
			aux->config_pos = i;
			cur_command = 2;
		}
		else if (cur_position == 0 && cur_command == 2)
		{
			aux->positional0 = argv[i];
			cur_command++;
		}
		else if (cur_position == 1 && cur_command == 2)
		{
			aux->positional1 = argv[i];
			cur_command++;
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

