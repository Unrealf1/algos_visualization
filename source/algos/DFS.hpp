#pragma once

#include <stack>

#include "search_algos_util.hpp"


namespace algos {
    template<
        std::equality_comparable Node,
        typename Neighboors,
        typename Predicate,
        template<typename> typename StackType = std::stack
    >
    requires NeighboorsGetter<Neighboors, Node> && NodePredicate<Predicate, Node>
    static NodePath<Node> DFSFindPath(
            const Node& from,
            const Predicate& is_searched,
            const Neighboors& get_neighboors
    ) {
        std::vector<ReconstructionItem<Node>> processed;
        StackType<ReconstructionItem<Node>> stack;
        stack.push({ from, 0 });
        while (!stack.empty()) {
            const auto& [current, parent] = stack.top();
            stack.pop();

            if (rng::find(processed, current, &ReconstructionItem<Node>::child) != processed.end()) {
                continue;
            }
            
            const auto my_index = processed.size();
            processed.push_back({current, parent});

            if (is_searched(current)) {
                return reconstruct_path(current, processed);
            }

            for (const Node& child : get_neighboors(current)) {
                stack.push({ child, my_index });
            }
        }
        return {};
    }
}
