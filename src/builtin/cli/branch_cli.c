#include <stdio.h>
#include <string.h>

struct cli
{
	int a;
	int branch;
	int help;
	char *pattern;
	int r;
};

struct cli_aux
{
	int a_cmd;
	int branch_pos;
	int help_cmd;
	char *positional0;
	int r_cmd;
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
	if (aux->r_cmd != 0 && aux->r_cmd != 2)
	{
		fprintf(stderr,"Option -r may be given only for the \"branch\" command\n");
		return 0;
	}
	if (aux->a_cmd != 0 && aux->a_cmd != 2)
	{
		fprintf(stderr,"Option -a may be given only for the \"branch\" command\n");
		return 0;
	}
	{
		int count = 0;
		count += !!cli->r;
		count += !!cli->a;
		if (count > 1)
		{
			fprintf(stderr, "Only one of -r or -a may be given\n");
			return 0;
		}
	}
	if (cli->branch)
	{
		cli->pattern = aux->positional0;
	}
	else
	{
		fprintf(stderr,"Please specify a proper command. Use --help for usage.\n");
		return 0;
	}
	return 1;
}

/**
 * Print usage for the given cli.
 *
 * @return 1 if usage has been printed, 0 otherwise.
 */
static int usage_cli(char *cmd, struct cli *cli)
{
	if (!cli->help)
	{
		return 0;
	}
	fprintf(stderr, "usage: %s <command> [<options>]\n", cmd);
	fprintf(stderr, "branch [-r | -a] [<pattern>]\n");
	return 1;
}

static int parse_cli_simple(int argc, char *argv[], struct cli *cli, struct cli_aux *aux)
{
	int i;
	int cur_command = -1;
	int cur_position = 0;
	for (i=0; i < argc; i++)
	{
		if (!strcmp("--help", argv[i]))
		{
			cli->help = 1;
			aux->help_cmd = cur_command;
		}
		else if (!strcmp("-a", argv[i]))
		{
			cli->a = 1;
			aux->a_cmd = cur_command;
		}
		else if (!strcmp("-r", argv[i]))
		{
			cli->r = 1;
			aux->r_cmd = cur_command;
		}
		else if (!strcmp("branch", argv[i]))
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
			fprintf(stderr,"Unknown command or option \"%s\"\n", argv[i]);
			return 0;
		}
	}
	return 1;
}

static int parse_cli(int argc, char *argv[], struct cli *cli, parse_cli_options_t opts)
{
	struct cli_aux aux;
	char *cmd = argv[0];
	memset(&aux, 0, sizeof(aux));
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

