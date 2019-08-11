/**
 * Automatically generated file, please don't edit!
 */
#include <stdio.h>
#include <string.h>

struct cli
{
	int a;
	int branch;
	int help;
	char *pattern;
	int r;
	int set_upstream_to;
	int unset_upstream;
	char *upstream;
};

struct cli_aux
{
	int a_cmd;
	int branch_pos;
	int help_cmd;
	char *positional0;
	int r_cmd;
	int set_upstream_to_pos;
	int unset_upstream_pos;
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
	if (aux->r_cmd != 0 && aux->r_cmd != 2)
	{
		fprintf(stderr, "Option -r may be given only for the \"branch\" command\n");
		return 0;
	}
	if (aux->a_cmd != 0 && aux->a_cmd != 2)
	{
		fprintf(stderr, "Option -a may be given only for the \"branch\" command\n");
		return 0;
	}
	if ((!!cli->r + !!cli->a) > (1))
	{
		fprintf(stderr, "Only one of -r or -a may be given\n");
		return 0;
	}
	if (cli->branch && cli->unset_upstream)
	{
	}
	else if (cli->branch && cli->set_upstream_to)
	{
	}
	else if (cli->branch)
	{
		cli->pattern = aux->positional0;
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
	fprintf(stderr, "branch --set-upstream-to=<upstream>\n");
	fprintf(stderr, "branch --unset-upstream\n");
	fprintf(stderr, "branch [-r | -a] [<pattern>]\n");
	return 1;
}

static int parse_cli_simple(int argc, char **argv, struct cli *cli, struct cli_aux *aux)
{
	int i;
	int cur_position = 0;
	int cur_command = -1;
	for (i=0; i < argc; i++)
	{
		if (!strcmp(argv[i], "--help"))
		{
			cli->help = 1;
			aux->help_cmd = cur_command;
		}
		else if (!strncmp("--set-upstream-to", argv[i], 16) && (argv[i][16]=='=' || !argv[i][16]))
		{
			cli->set_upstream_to = 1;
			aux->set_upstream_to_pos = i;
			cur_command = 4;

			if (!argv[i][16])
			{
				if (((i) + (1)) < (argc))
				{
					cli->upstream = argv[(i) + (1)];
					i++;
				}
				else
				{
					fprintf(stderr, "Argument \"--set-upstream-to\" requires a value\n");
					return 0;
				}
			}
			else
			{
				cli->upstream = &argv[i][17];
			}
		}
		else if (!strcmp(argv[i], "--unset-upstream"))
		{
			cli->unset_upstream = 1;
			aux->unset_upstream_pos = i;
			cur_command = 3;
		}
		else if (!strcmp(argv[i], "-a"))
		{
			cli->a = 1;
			aux->a_cmd = cur_command;
		}
		else if (!strcmp(argv[i], "-r"))
		{
			cli->r = 1;
			aux->r_cmd = cur_command;
		}
		else if (!strcmp(argv[i], "branch"))
		{
			cli->branch = 1;
			aux->branch_pos = i;
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

