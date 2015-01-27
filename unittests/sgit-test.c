#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#include "graph.c"

struct node
{
	int nparent;
	int parents[10];
	char *text;
};

struct node g[] =
{
	{1,{1},"Head"},			/* 0 */
	{1,{2},"Head~1"},		/* 1 */
	{1,{3},"Head~2"},		/* 2 */
	{2,{4,6},"Merge"},		/* 3 */
	{1,{5},"Branch1"},		/* 4 */
	{1,{7},"Branch1~1"},	/* 5 */
	{1,{7},"Branch2"},		/* 6 */
	{1,{8},"Common"},		/* 7 */
	{0,{},"Common~1"}		/* 8 */
};

static node_t get_node(int index, void *userdata)
{
	return (intptr_t)index;
}

static int get_num_parents(node_t node, void *userdata)
{
	return g[(intptr_t)node].nparent;
}

static node_t get_parent(node_t node, int index, void *userdata)
{
	return g[(intptr_t)node].parents[index];
}

static const char *get_text(node_t node, void *userdata)
{
	return g[(intptr_t)node].text;
}

int main(int argc, char **argv)
{
	graph_callbacks callbacks = {0};

	callbacks.get_node = get_node;
	callbacks.get_num_parents = get_num_parents;
	callbacks.get_parent = get_parent;
	callbacks.get_text = get_text;

	graph_render(sizeof(g)/sizeof(g[0]), &callbacks);

	return 0;
}
