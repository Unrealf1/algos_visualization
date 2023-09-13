#pragma once

#include "a_star.hpp"


namespace algos {
    template<
        std::equality_comparable Node,
        typename Neighboors,
        typename Predicate,
        typename Weight,
        typename Reconstructor = decltype(reconstruct_path<Node>)
    >
    requires NeighboorsGetter<Neighboors, Node> 
        && WeightGetter<Weight, Node> 
        && NodePredicate<Predicate, Node>
    static NodePath<Node> DijkstraFindPath(
            const Node& from,
            const Predicate& is_searched,
            const Neighboors& get_neighboors,
            const Weight& get_weight,
            const Reconstructor& reconstructor = reconstruct_path<Node>
    ) {
        return AStarFindPath(from, is_searched, get_neighboors, get_weight, [](const Node&) { return 0.0; }, reconstructor);
    }
}

