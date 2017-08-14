#include <stdio.h>
#include <string.h>

struct cli
{
	int help;
	int help_cmd;
	int init;
	int init_pos;
	int update;
	int update_pos;
};

static int parse_cli(int argc, char *argv[], struct cli *cli)
{
	int i;
	int cur_command = -1;
	int cur_position = 0;
	for (i=0; i < argc; i++)
	{
		if (!strcmp("--help", argv[i]))
		{
			cli->help = 1;
			cli->help_cmd = cur_command;
		}
		else if (!strcmp("init", argv[i]))
		{
			cli->init = 1;
			cli->init_pos = i;
			cur_command = 2;
		}
		else if (!strcmp("update", argv[i]))
		{
			cli->update = 1;
			cli->update_pos = i;
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

static int validate_cli(struct cli *cli)
{
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
