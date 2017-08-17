#include <stdio.h>
#include <string.h>

struct cli
{
	int a;
	int a_cmd;
	int branch;
	int branch_pos;
	int help;
	int help_cmd;
	char *pattern;
	char *positional0;
	int r;
	int r_cmd;
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
		else if (!strcmp("-a", argv[i]))
		{
			cli->a = 1;
			cli->a_cmd = cur_command;
		}
		else if (!strcmp("-r", argv[i]))
		{
			cli->r = 1;
			cli->r_cmd = cur_command;
		}
		else if (!strcmp("branch", argv[i]))
		{
			cli->branch = 1;
			cli->branch_pos = i;
			cur_command = 2;
		}
		else if (cur_position == 0 && cur_command == 2)
		{
			cli->positional0 = argv[i];
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

static int validate_cli(struct cli *cli)
{
	if (cli->r_cmd != 0 && cli->r_cmd != 2)
	{
		fprintf(stderr,"Option -r may be given only for the \"branch\" command\n");
		return 0;
	}
	if (cli->a_cmd != 0 && cli->a_cmd != 2)
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
		cli->pattern = cli->positional0;
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
	fprintf(stderr, "branch [-r | -a] [<pattern>]\n");
	return 1;
}
