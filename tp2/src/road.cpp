#include "road.h"

#include <iostream>
#include <vector>
#include <string>
#include <list>

#include <limits> // for numeric_limits

#include <queue>
#include <utility> // for pair
#include <algorithm>
#include <iterator>

const weight_t max_weight = std::numeric_limits<double>::infinity();

neighbor& neighbor::operator=(const neighbor &rhs)
{
    if (this != &rhs)
    {
        target = rhs.target;
        weight = rhs.target;
    }

    return *this;
}

Road::Road(const HeightField &hf, int _k) : HeightField(hf), k(_k) {}

std::list<std::pair<int, int>> Road::compute(vertex_t source)
{
    std::vector<weight_t> min_distance;
    std::vector<vertex_t> previous;
    DijkstraComputePaths(source, min_distance, previous);

    vertex_t vertex = source;
    std::list<std::pair<int, int>> path;
    for (; vertex != -1; vertex = previous[vertex])
        path.push_front(inverseIndex(vertex));
    return path;
}

double Road::slope_transfer(double s)
{
    if (s > max_slope)
        return max_weight;
    return s;
}

std::vector<neighbor> Road::connexity(vertex_t u)
{
    std::pair<int, int> p = inverseIndex(u);
    int i = p.first, j = p.second;
    std::vector<neighbor> neighbors;
    neighbors.resize(k * k);

    for (int ii = -k; ii < k + 1; ++ii)
        for (int jj = -k; jj < k + 1; ++jj)
        {
            int ki = i + ii;
            int kj = j + jj;
            if (inside(ki, kj) == true)
            {
                weight_t w = 1.0;
                w += slope_transfer(slope(ki, kj));
                neighbors.push_back(neighbor(index(ki, kj), w));
            }
        }

    return neighbors;
}

// typedef std::vector<std::vector<neighbor>> adjacency_list_t;
typedef std::pair<weight_t, vertex_t> weight_vertex_pair_t;

void Road::DijkstraComputePaths(vertex_t source,
                                std::vector<weight_t> &min_distance,
                                std::vector<vertex_t> &previous)
{
    // int n = adjacency_list.size();
    int n = nx * ny;
    min_distance.clear();
    min_distance.resize(n, max_weight);
    min_distance[source] = 0;
    previous.clear();
    previous.resize(n, -1);

    // we use greater instead of less to turn max-heap into min-heap
    std::priority_queue<weight_vertex_pair_t,
                        std::vector<weight_vertex_pair_t>,
                        std::greater<weight_vertex_pair_t>>
        vertex_queue;
    vertex_queue.push(std::make_pair(min_distance[source], source));

    while (!vertex_queue.empty())
    {
        weight_t dist = vertex_queue.top().first;
        vertex_t u = vertex_queue.top().second;
        vertex_queue.pop();

        // Because we leave old copies of the vertex in the priority queue
        // (with outdated higher distances), we need to ignore it when we come
        // across it again, by checking its distance against the minimum distance
        if (dist > min_distance[u])
            continue;

        // Visit each edge exiting u
        // const std::vector<neighbor> &neighbors = adjacency_list[u];
        std::vector<neighbor> neighbors = connexity(u);

        for (std::vector<neighbor>::const_iterator neighbor_iter = neighbors.begin();
             neighbor_iter != neighbors.end();
             neighbor_iter++)
        {
            vertex_t v = neighbor_iter->target;
            weight_t weight = neighbor_iter->weight;
            weight_t distance_through_u = dist + weight;
            if (distance_through_u < min_distance[v])
            {
                min_distance[v] = distance_through_u;
                previous[v] = u;
                vertex_queue.push(std::make_pair(min_distance[v], v));
            }
        }
    }
}

// std::list<vertex_t> Road::DijkstraGetShortestPathTo(
//     vertex_t vertex, const std::vector<vertex_t> &previous)
// {
//     std::list<vertex_t> path;
//     for (; vertex != -1; vertex = previous[vertex])
//         path.push_front(vertex);
//     return path;
// }

// int main()
// {
//     // remember to insert edges both ways for an undirected graph
//     adjacency_list_t adjacency_list(6);
//     // 0 = a
//     adjacency_list[0].push_back(neighbor(1, 7));
//     adjacency_list[0].push_back(neighbor(2, 9));
//     adjacency_list[0].push_back(neighbor(5, 14));
//     // 1 = b
//     adjacency_list[1].push_back(neighbor(0, 7));
//     adjacency_list[1].push_back(neighbor(2, 10));
//     adjacency_list[1].push_back(neighbor(3, 15));
//     // 2 = c
//     adjacency_list[2].push_back(neighbor(0, 9));
//     adjacency_list[2].push_back(neighbor(1, 10));
//     adjacency_list[2].push_back(neighbor(3, 11));
//     adjacency_list[2].push_back(neighbor(5, 2));
//     // 3 = d
//     adjacency_list[3].push_back(neighbor(1, 15));
//     adjacency_list[3].push_back(neighbor(2, 11));
//     adjacency_list[3].push_back(neighbor(4, 6));
//     // 4 = e
//     adjacency_list[4].push_back(neighbor(3, 6));
//     adjacency_list[4].push_back(neighbor(5, 9));
//     // 5 = f
//     adjacency_list[5].push_back(neighbor(0, 14));
//     adjacency_list[5].push_back(neighbor(2, 2));
//     adjacency_list[5].push_back(neighbor(4, 9));

//     std::vector<weight_t> min_distance;
//     std::vector<vertex_t> previous;
//     DijkstraComputePaths(0, adjacency_list, min_distance, previous);
//     std::cout << "Distance from 0 to 4: " << min_distance[4] << std::endl;
//     std::list<vertex_t> path = DijkstraGetShortestPathTo(4, previous);
//     std::cout << "Path : ";
//     std::copy(path.begin(), path.end(), std::ostream_iterator<vertex_t>(std::cout, " "));
//     std::cout << std::endl;

//     return 0;
// }
