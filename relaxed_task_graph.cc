#include "relaxed_task_graph.h"

#include <iostream>
#include <vector>

using namespace std;

namespace planopt_heuristics {
RelaxedTaskGraph::RelaxedTaskGraph(const TaskProxy &task_proxy)
    : relaxed_task(task_proxy),
      variable_node_ids(relaxed_task.propositions.size()) {

	/* add init AND node */
	NodeID init_and = graph.add_node(NodeType::AND);

	/* add variable's OR nodes */
	for (Proposition p : relaxed_task.propositions) {
		NodeID operator_or = graph.add_node(NodeType::OR);
		variable_node_ids[p.id] = operator_or;
	}

	/* add edges from operator nodes to init node */
	for (PropositionID id : relaxed_task.initial_state) {
		graph.add_edge(variable_node_ids[id], init_and);
	}

	/* for every operator add AND node */
	for (RelaxedOperator o : relaxed_task.operators) {
		NodeID operator_and = graph.add_node(NodeType::AND);

		/* add edges to precond */
		for (PropositionID id : o.preconditions) {
			graph.add_edge(operator_and, variable_node_ids[id]);
		}
		/* add edges from effects */
		for (PropositionID id : o.effects) {
			graph.add_edge(variable_node_ids[id], operator_and);
		}
	}

	/* add goal node and belonging edges */
	NodeID goal_and = graph.add_node(NodeType::AND);
	for (PropositionID id : relaxed_task.goal) {
		graph.add_edge(goal_and, variable_node_ids.at(id));
	}
	
	/* set init_node and goal_node */
	initial_node_id = init_and;
	goal_node_id = goal_and;

	  
    /*
      TODO: add your code for exercise 2 (b) here. Afterwards
        - variable_node_ids[i] should contain the node id of the variable node for variable i
        - initial_node_id should contain the node id of the initial node
        - goal_node_id should contain the node id of the goal node
        - the graph should contain precondition and effect nodes for all operators
        - the graph should contain all necessary edges.
    */
}

void RelaxedTaskGraph::change_initial_state(const GlobalState &global_state) {
    // Remove all initial edges that where introduced for relaxed_task.initial_state.
    for (PropositionID id : relaxed_task.initial_state) {
        graph.remove_edge(variable_node_ids[id], initial_node_id);
    }

    // Switch initial state of relaxed_task
    relaxed_task.initial_state = relaxed_task.translate_state(global_state);

    // Add all initial edges for relaxed_task.initial_state.
    for (PropositionID id : relaxed_task.initial_state) {
        graph.add_edge(variable_node_ids[id], initial_node_id);
    }
}

bool RelaxedTaskGraph::is_goal_relaxed_reachable() {
	graph.most_conservative_valuation();

	if (graph.get_node(goal_node_id).forced_true) {
		return true;
	}
	return false;


    // Compute the most conservative valuation of the graph and use it to
    // return true iff the goal is reachable in the relaxed task.
}

int RelaxedTaskGraph::additive_cost_of_goal() {
    // Compute the weighted most conservative valuation of the graph and use it
    // to return the h^add value of the goal node.

    // TODO: add your code for exercise 2 (d) here.
    return -1;
}

int RelaxedTaskGraph::ff_cost_of_goal() {
    // TODO: add your code for exercise 2 (f) here.
    return -1;
}

}
