#include <stdio.h>
#include <string.h>

struct cli
{
	int help;
	int init;
	int update;
};

struct cli_aux
{
	int help_cmd;
	int init_pos;
	int update_pos;
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
	if (cli->init)
	{
	}
	else if (cli->update)
	{
	}
	else
	{
		fprintf(stderr,"Please specify a proper command. Use --help for usage.\n");
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
	fprintf(stderr, "init\n");
	fprintf(stderr, "update\n");
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
		else if (!strcmp("init", argv[i]))
		{
			cli->init = 1;
			aux->init_pos = i;
			cur_command = 2;
		}
		else if (!strcmp("update", argv[i]))
		{
			cli->update = 1;
			aux->update_pos = i;
			cur_command = 3;
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

