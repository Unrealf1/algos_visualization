#pragma once

#include <algorithm>
#include <bits/ranges_algo.h>
#include <iterator>

#include "search_algos_util.hpp"


namespace algos {
    template<
        std::equality_comparable Node,
        typename Neighboors,
        typename Weight
    >
    requires NeighboorsGetter<Neighboors, Node> && WeightGetter<Weight, Node>
    static NodePath<Node> DijkstraFindPath(
            const Node& from,
            const Node& to,
            const Neighboors& get_neighboors,
            const Weight& get_weight
    ) {
        struct LengthEstimate {
            Node node;
            double estimate;
            size_t parent;
            size_t my_index;
        };
        std::vector<LengthEstimate> estimates = { {from, 0.0, 0, 0} };
        std::vector<size_t> unvisited_indices = { 0 };

        auto exit_condition = [&] {
            return unvisited_indices.empty();
        };

        while (!exit_condition()) {
            auto current_it = rng::min_element(
                unvisited_indices,
                {},
                [&](size_t idx) { return estimates[idx].estimate; }
            );
            auto current = estimates[*current_it];

            if (current.node == to) {
                std::vector<ReconstructionItem<Node>> parents;
                parents.reserve(estimates.size());
                rng::transform(estimates, std::back_inserter(parents), [](const LengthEstimate& item) {
                    return ReconstructionItem{item.node, item.parent};
                });

                return reconstruct_path(to, parents);
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

