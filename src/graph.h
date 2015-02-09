#ifndef SIMPLEGIT_GRAPH_H
#define SIMPLEGIT_GRAPH_H

typedef void * node_t;

/**
 * State for each cell in the rendered row.
 */
typedef enum
{
	/** There is an edge from left (center) to the center */
	LEFT_TO_CENTER = 1<<0,

	/** There is an edge from top (center) to the center */
	TOP_TO_CENTER = 1<<1,

	/** There is an edge from right (center) to the center */
	RIGHT_TO_CENTER = 1<<2,
	CENTER_TO_RIGHT = RIGHT_TO_CENTER,

	/** There is an edge from bottom (center) to the center */
	BOTTOM_TO_CENTER = 1<<3,

	/** Put in a node */
	NODE = 1<<4,

	/** There is an edge from the top (center) to the bottom (center) */
	TOP_TO_BOTTOM = (TOP_TO_CENTER | BOTTOM_TO_CENTER),

	/** There is an edge from the left (center) to the right (center) */
	LEFT_TO_RIGHT = (LEFT_TO_CENTER | RIGHT_TO_CENTER),
} cell_state_t;

/**
 * This basically describes an edge set.
 */
struct graph_print_data
{
	int num_aboves;
	int *aboves;
	int collapse_goal;
	int num_collapsing;
	int *collapsing;
};

typedef struct
{
	/** Returns the node at the given index */
	node_t (*get_node)(int index, void *userdata);

	/** Returns the number of parents for the given node */
	int (*get_num_parents)(node_t node, void *userdata);

	/** Returns the index'th node of the given node starting with 0 */
	node_t (*get_parent)(node_t node, int index, void *userdata);

	/** The client shall print the given cell state */
	void (*print_row)(node_t node, cell_state_t *states, int num, void *userdata);

	/** The client shall print the given row */
	void (*new_print_row)(node_t node, struct graph_print_data *data, void *userdata);

	/** The user data that is supplied to various callbacks */
	void *userdata;
} graph_callbacks;

/**
 * Start the rendering of a graph at node given at head.
 */
void graph_render(int num_nodes, graph_callbacks *callbacks);

#endif

