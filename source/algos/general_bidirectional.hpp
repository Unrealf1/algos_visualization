#pragma once

#include "search_algos_util.hpp"
#include <utility>


namespace algos {

// VGA stands for Very General Algorithm. Please refer complaints
// about naming to IRA POHL. But to be fair, it was 1969.
template<
    std::equality_comparable Node,
    typename Neighboors,
    typename Weight,
    typename FrontSelector,
    typename Reconstructor = decltype(reconstruct_path<Node>)
>
requires NeighboorsGetter<Neighboors, Node>
static NodePath<Node> VGAFindPath(
        const Node& from,
        const Node& to,
        const Neighboors& get_neighboors,
        const Weight& get_weight,
        const FrontSelector& front_selector,
        const Reconstructor& reconstructor = reconstruct_path<Node>
) {
  struct PathLengthEstimate {
      Node node;
      double best_path_length;
      size_t parent;
      size_t my_index;
  };

  struct Front {
    std::vector<PathLengthEstimate> estimates;
    std::vector<size_t> unvisited_indices;
  };
  Front startFront = {
    .estimates = { {from, 0.0, 0, 0} },
    .unvisited_indices = { 0 }
  };

  Front endFront = {
    .estimates = { {to, 0.0, 0, 0} },
    .unvisited_indices = { 0 }
  };

  while (true) {
    bool selectFirst = front_selector(startFront, endFront);
    auto& front = selectFirst ? startFront : endFront;
    const auto& inactiveFront = !selectFirst ? startFront : endFront;
    if (front.unvisited_indices.empty()) {
      return {};
    }

    auto current_it = rng::min_element(
      front.unvisited_indices,
      {},
      [&](size_t idx) { return front.estimates[idx].best_path_length; }
    );
    auto minValue = front.estimates[*current_it].best_path_length;
    std::vector minElementIdxIters {current_it};
    for (auto it = (current_it + 1); it != front.unvisited_indices.end(); ++it) {
      if (front.estimates[*it].best_path_length == minValue) {
        minElementIdxIters.push_back(it);
      }
    }

    std::vector<size_t> newUnvisitedIndices; // TODO: reserve something?
    struct Solution {
      double estimatedCost;
      size_t centralNodeIdxActiveFront;
      size_t centralNodeIdxInactiveFront;
    };
    std::vector<Solution> solutionsFound;
    for (int i = minElementIdxIters.size() - 1; i >= 0 ; --i) {
      auto minElementIdxIter = minElementIdxIters[i];
      auto minElementIdx = *minElementIdxIter;
      auto current = front.estimates[minElementIdx];

      for (const auto& otherEstimate : inactiveFront.estimates) {
        if (otherEstimate.node == current.node) {
          solutionsFound.emplace_back(
              current.best_path_length + otherEstimate.best_path_length,
              current.my_index,
              otherEstimate.my_index);
        }
      }

      if (!solutionsFound.empty()) {
        auto bestSolutionIt = rng::min_element(
          solutionsFound,
          {},
          &Solution::estimatedCost
        );

        std::vector<ReconstructionItem<Node>> parents;
        parents.reserve(front.estimates.size());
        rng::transform(front.estimates, std::back_inserter(parents), [](const PathLengthEstimate& item) {
          return ReconstructionItem{item.node, item.parent};
        });

        const auto& node = front.estimates[bestSolutionIt->centralNodeIdxActiveFront].node;
        auto path1 = reconstructor(node, parents);
        parents.clear();
        parents.reserve(inactiveFront.estimates.size());
        rng::transform(inactiveFront.estimates, std::back_inserter(parents), [](const PathLengthEstimate& item) {
          return ReconstructionItem{item.node, item.parent};
        });
        auto path2 = reconstructor(node, parents);
        NodePath<Node> result;
        result.reserve(path1.size() + path2.size() - 1);
        if (*path1.rbegin() == from) {
          // path1: center..from
          // path2: center..to
          // want : to..center..from
          result.insert(result.begin(), path2.rbegin(), path2.rend());
          // now result is to..center
          result.insert(result.end(), path1.begin() + 1, path1.end());
        } else {
          // path1: center..to
          // path2: center..from
          result.insert(result.begin(), path1.rbegin(), path1.rend());
          result.insert(result.end(), path2.begin() + 1, path2.end());
        }
        return result;
      }

      const auto& neighboors = get_neighboors(current.node);
      for (const auto& neighboor : neighboors) {
        auto it = rng::find(front.estimates, neighboor, &PathLengthEstimate::node);
        const auto edge_path_weight = current.best_path_length + get_weight(current.node, neighboor);
        if (it == front.estimates.end()) {
          newUnvisitedIndices.push_back(front.estimates.size());
          front.estimates.emplace_back(neighboor, edge_path_weight, current.my_index, front.estimates.size());
        } else {
          if (edge_path_weight < it->best_path_length) {
            it->best_path_length = edge_path_weight;
            it->parent = current.my_index;
          }
        }
      }

      front.unvisited_indices.erase(minElementIdxIter); // TODO: ineffective, should remove all at once
      // Prepare iter for delayed removal
      // std::swap(*minElementIdxIter, *(front.estimates.rbegin() + (minElementIdxIters.size() - i + 1)));
    }

    //front.unvisited_indices.erase(
    //  front.unvisited_indices.end() - minElementIdxIters.size(),
    //  front.unvisited_indices.end()
    //);
    front.unvisited_indices.insert(
        front.unvisited_indices.end(),
        newUnvisitedIndices.begin(),
        newUnvisitedIndices.end()
    );
  }
  // UNREACHABLE
  return {};
}
}
