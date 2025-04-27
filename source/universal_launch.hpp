#pragma once

#include <algos/BFS.hpp>
#include <algos/DFS.hpp>
#include <algos/dijkstra.hpp>
#include <algos/a_star.hpp>
#include <iterator>
#include <parameters.hpp>
#include <maze.hpp>
#include <ranges>


namespace algos {
    struct LoggedResult {
        NodePath<Maze::Node> path;
        std::vector<Maze::Node> search_log;
    };
    
    LoggedResult run_logged_algorithm(ApplicationParams::EAlgorithm algorithm, const Maze& maze) {
        LoggedResult result;
        auto edge_getter = [&](const Maze::Node& node) {
            std::vector<Maze::Node> result;
            std::ranges::copy_if(maze.get_neighboors(node), std::back_inserter(result), [&](Maze::Node nd) {
                return maze.get_cell(nd) != MazeObject::wall;
            });
            return result;
        };
        auto logging_searcher = [&](const Maze::Node& node) {
            result.search_log.push_back(node);
            return node == maze.finish;
        };
        result.path = [&]{
            switch (algorithm) {
                case ApplicationParams::EAlgorithm::BFS: {
                    return BFSFindPath<Maze::Node>(maze.start, logging_searcher, edge_getter);
                }
                case ApplicationParams::EAlgorithm::DFS: {
                    return DFSFindPath<Maze::Node>(maze.start, logging_searcher, edge_getter);
                }
                case ApplicationParams::EAlgorithm::Dijkstra: {
                    return DijkstraFindPath(maze.start, logging_searcher, edge_getter, [](const auto&, const auto&) {
                        return 1.0;
                    });
                }
                case ApplicationParams::EAlgorithm::AStar: {
                    return AStarFindPath(maze.start, logging_searcher, edge_getter, [](const auto&, const auto&) {
                        return 1.0;
                    }, [&](const Maze::Node& node) {
                        auto dx = node.x - maze.finish.x;
                        auto dy = node.y - maze.finish.y;
                        return std::sqrt(dx * dx + dy * dy);
                    });
                }
            }
            // should not be reachable. Kept here for now because of gcc warning(end of non-void finction)
            throw std::logic_error("Unknown algorithm!");
        }();
        return result;
    }
}
