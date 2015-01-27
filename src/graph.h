#ifndef SIMPLEGIT_GRAPH_H
#define SIMPLEGIT_GRAPH_H

typedef void * node_t;

typedef struct
{
	/** Returns the node at the given index */
	node_t (*get_node)(int index, void *userdata);

	/** Returns the number of parents for the given node */
	int (*get_num_parents)(node_t node, void *userdata);

	/** Returns the index'th node of the given node starting with 0 */
	node_t (*get_parent)(node_t node, int index, void *userdata);

	/** Returns the text for the given node */
	const char * (*get_text)(node_t node, void *userdata);

	/** The user data that is supplied to various callbacks */
	void *userdata;
} graph_callbacks;

/**
 * Start the rendering of a graph at node given at head.
 */
void graph_render(int num_nodes, graph_callbacks *callbacks);

#endif

