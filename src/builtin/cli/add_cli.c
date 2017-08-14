#include <stdio.h>
#include <string.h>

struct cli
{
	int add;
	int add_pos;
	int help;
	int help_cmd;
	char **pathspec;
	int pathspec_num;
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
		else if (!strcmp("add", argv[i]))
		{
			cli->add = 1;
			cli->add_pos = i;
			cur_command = 2;
		}
		else if (cur_position == 0)
		{
			cli->pathspec = &argv[i];
			cli->pathspec_num = argc - i;
			break;
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
	if (cli->add)
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
	fprintf(stderr, "add [<pathspec>...]\n");
	return 1;
}
