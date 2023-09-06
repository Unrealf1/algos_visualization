#pragma once

#include <algorithm>
#include <iterator>

#include "search_algos_util.hpp"


namespace algos {
    template<
        std::equality_comparable Node,
        typename Neighboors,
        typename Predicate,
        typename Weight,
        typename Heuristic
    >
    requires NeighboorsGetter<Neighboors, Node> 
        && WeightGetter<Weight, Node> 
        && NodePredicate<Predicate, Node>
        && HeuristicGetter<Heuristic, Node>
    static NodePath<Node> AStarFindPath(
            const Node& from,
            const Predicate& is_searched,
            const Neighboors& get_neighboors,
            const Weight& get_weight,
            const Heuristic& get_heuristic
    ) {
        struct LengthEstimate {
            Node node;
            double estimate;
            size_t parent;
            size_t my_index;
        };
        std::vector<LengthEstimate> estimates = { {from, 0.0, 0, 0} };
        std::vector<size_t> unvisited_indices = { 0 };

        while (!unvisited_indices.empty()) {
            // TODO: maybe store heuristic in the LengthEstimate?
            // Will save computation, loose some memory and weird cases
            // where heuristic can change
            auto current_it = rng::min_element(
                unvisited_indices,
                {},
                [&](size_t idx) { return estimates[idx].estimate + get_heuristic(estimates[idx].node); }
            );
            auto current = estimates[*current_it];

            if (is_searched(current.node)) {
                std::vector<ReconstructionItem<Node>> parents;
                parents.reserve(estimates.size());
                rng::transform(estimates, std::back_inserter(parents), [](const LengthEstimate& item) {
                    return ReconstructionItem{item.node, item.parent};
                });

                return reconstruct_path(current.node, parents);
            }

            unvisited_indices.erase(current_it);

            const auto& neighboors = get_neighboors(current.node);
            for (const auto& neighboor : neighboors) {
                auto it = rng::find(estimates, neighboor, &LengthEstimate::node);
                const auto edge_path_weight = current.estimate + get_weight(current.node, neighboor);
                if (it == estimates.end()) {
                    unvisited_indices.push_back(estimates.size());
                    estimates.emplace_back(neighboor, edge_path_weight, current.my_index, estimates.size());
                } else {
                    if (edge_path_weight < it->estimate) {
                        it->estimate = edge_path_weight;
                        it->parent = current.my_index;
                    }
                }
            }
        }
        return {};
    }
}

