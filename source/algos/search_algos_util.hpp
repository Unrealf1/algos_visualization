#pragma once

#include <ranges>
#include <concepts>
#include <vector>


namespace algos {
    namespace rng = std::ranges;

    template<typename T, typename Node>
    concept NeighboorsGetter = requires(T getter, Node node) {
        { getter(node) } -> std::ranges::range;
    };

    template<typename T, typename Node>
    concept HeuristicGetter = requires(T heuristic, Node node) {
        { heuristic(node) } -> std::floating_point;
    };

    template<typename T, typename Node>
    concept NodePredicate = requires(T predicate, Node node) {
        { predicate(node) } -> std::same_as<bool>;
    };

    template<typename Node>
    struct Equals {
        Node to;
        bool operator()(const Node& other) const {
            return to == other;
        }
    };

    template<typename T, typename Node>
    concept WeightGetter = requires(T getter, Node node) {
        { getter(node, node) } -> std::floating_point;
    };

    template<typename Node>
    using NodePath = std::vector<Node>;

    template<typename Node>
    struct EmptyUpdate {
        void operator()(const Node&) const noexcept {}
    };

    template<typename Node>
    struct ReconstructionItem {
        const Node child;
        const size_t parent_index;
    };

    template<typename Node>
    NodePath<Node> reconstruct_path(const Node& finish, const std::vector<ReconstructionItem<Node>>& parents) {
        NodePath<Node> result = {finish};
        auto current_iter = rng::find(parents, finish, &ReconstructionItem<Node>::child);
        auto current_index = size_t(current_iter - parents.begin());
        auto parent_index = current_iter->parent_index;
        while (parent_index != current_index) {
            const auto& parent = parents[parent_index];
            result.push_back(parent.child);
            current_index = parent_index;
            parent_index = parent.parent_index;
        }
        return result;
    }
}
