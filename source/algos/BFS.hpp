#pragma once

#include <bits/ranges_util.h>
#include <queue>
#include <ranges>
#include <concepts>


namespace algos {
    namespace rng = std::ranges;

    template<typename T, typename Node>
    concept NeighboorsGetter = requires(T getter, Node node) {
        { getter(node) } -> std::ranges::range;
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

    template<
        std::equality_comparable Node,
        typename Neighboors,
        typename OnUpdate = EmptyUpdate<Node>,
        template<typename> typename QueueType = std::queue
    >
    requires NeighboorsGetter<Neighboors, Node>
    static NodePath<Node> BFSFindPath(
            const Node& from,
            const Node& to,
            const Neighboors& get_neighboors,
            const OnUpdate& update_callback = OnUpdate()
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

            update_callback(current);

            if (current == to) {
                return reconstruct_path(current, parents);
            }
            
            for (const Node& child : get_neighboors(current)) {
                auto this_node = rng::find(parents, child, &ReconstructionItem<Node>::child);
                if (this_node != parents.end()) {
                    continue;
                }
                que.push({ child, size_t(this_node - parents.begin()) });
                parents.push_back( {child, my_index} );
            }

            que.pop();
        }
        return {};
    }
}

