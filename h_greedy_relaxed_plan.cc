#include "h_greedy_relaxed_plan.h"

#include "../option_parser.h"
#include "../plugin.h"

#include <algorithm>

using namespace std;

namespace planopt_heuristics {
GreedyRelaxedPlanHeuristic::GreedyRelaxedPlanHeuristic(const options::Options &options)
    : Heuristic(options),
      relaxed_task(task_proxy) {
}

int GreedyRelaxedPlanHeuristic::compute_heuristic(const GlobalState &state) {
    relaxed_task.initial_state = relaxed_task.translate_state(state);

    vector<bool> proposition_reached(relaxed_task.propositions.size(), false);
    vector<bool> operartor_used(relaxed_task.operators.size(), false);
    for (PropositionID id : relaxed_task.initial_state) {
        proposition_reached[id] = true;
    }

    // TODO: add your code for exercise 1 (c) here.

    // We want to apply the first applicable operator that we encounter, provided it adds some new proposition
    // Each time that we do this, we should iterate again over all operators, applying the first applicable one
    // We stop looping when:
    //      - The goal state is reached
    //      - There are no more changes made (e.g. all applicable operators do not add anything new)

    bool bLooping = true;
    bool bAppliedRule = false;
    int cost = 0;

    while (bLooping) {
        // Reset bAppliedRule
        bAppliedRule = false;
        
        // First, check if we have reached the goal state.  If we have, return the cost of the plan
        // Iterate over all goal propositions. If they are contained in our proposition_reached vector, we are in the goal
        bool bInGoal = true;

        for (PropositionID pID : relaxed_task.goal) {
            if (! proposition_reached[pID]) {
                bInGoal = false;
                break;
            }
        }

        if (bInGoal) {
            return cost;
        }

        // If we are not in a goal state, check each operator
        for (RelaxedOperator op : relaxed_task.operators) {
            // If we have already applied this operator, we do not need to apply it again, since it does not give us any new propositions
            if (operartor_used[op.id]) {
                continue;
            }

            // Determine if it is applicable by ensuring that all preconditions are in our proposition_reached vector
            bool bOperatorApplicable = true;
            for (PropositionID pID : op.preconditions) {
                if (! proposition_reached[pID]) {
                    bOperatorApplicable = false;
                    break;
                }
            }

            if (! bOperatorApplicable) {
                continue;
            }

            // If we reach here, operator is applicable
            // Now we should check that it actually adds something useful to our list of reached propositions
            bool bOperatorAddsValue = false;

            for (PropositionID pID : op.effects) {
                if (! proposition_reached[pID]) {
                    bOperatorAddsValue = true;
                    break;
                }
            }

            if (! bOperatorAddsValue) {
                continue;
            }

            // If we reach here, the operator adds value.  We will now "apply" the operator
            for (PropositionID pID : op.effects) {
                proposition_reached[pID] = true;
            }

            operartor_used[op.id] = true;
            cost += op.cost;
            bAppliedRule = true;

            // Now, we have applied an operator, which means that we should exit this loop and start again with our new state
            break;
        }

        // If we applied an operator, then continue looping.  If we did not apply an operator, and we are not in a goal state
        // We have run out of operators to apply, and thus we are in a dead end.
        if (! bAppliedRule) {
            bLooping = false;
        }
    }

    return DEAD_END;
}

static Heuristic *_parse(OptionParser &parser) {
    Heuristic::add_options_to_parser(parser);
    Options opts = parser.parse();
    if (parser.dry_run())
        return 0;
    else
        return new GreedyRelaxedPlanHeuristic(opts);
}

static Plugin<Heuristic> _plugin("planopt_greedy_relaxed", _parse);

}
