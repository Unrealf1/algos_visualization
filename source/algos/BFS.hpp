#pragma once

#include <queue>

#include "search_algos_util.hpp"


namespace algos {
    template<
        std::equality_comparable Node,
        typename Neighboors,
        typename Predicate,
        typename Reconstructor = decltype(reconstruct_path<Node>),
        template<typename> typename QueueType = std::queue
    >
    requires NeighboorsGetter<Neighboors, Node> && NodePredicate<Predicate, Node>
    static NodePath<Node> BFSFindPath(
            const Node& from,
            const Predicate& is_searched,
            const Neighboors& get_neighboors,
            const Reconstructor& reconstructor = reconstruct_path<Node>
    ) {
        struct QueItem {
            const Node child;
            const size_t my_index;
        };

        QueueType<QueItem> que;
        std::vector<ReconstructionItem<Node>> parents = { {from, 0} }; 
        que.push({ from, 0 });
        while (!que.empty()) {
            const auto& [current, my_index] = que.front();

            if (is_searched(current)) {
                return reconstructor(current, parents);
            }
            
            for (const Node& child : get_neighboors(current)) {
                auto this_node = rng::find(parents, child, &ReconstructionItem<Node>::child);
                if (this_node != parents.end()) {
                    continue;
                }
                que.push({ child, parents.size() });
                parents.push_back({ child, my_index });
            }

            que.pop();
        }
        return {};
    }
}

