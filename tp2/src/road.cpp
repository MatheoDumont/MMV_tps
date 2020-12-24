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

neighbor &neighbor::operator=(const neighbor &rhs)
{
    if (this != &rhs)
    {
        target = rhs.target;
        weight = rhs.target;
    }

    return *this;
}

Road::Road(const HeightField &hf, int _k) : HeightField(hf), k(_k) {}

double Road::slope_transfer(double s) const
{
    if (s > max_slope)
        return max_weight;
    return s;
}

std::vector<neighbor> Road::connexity(int i, int j) const
{
    std::vector<neighbor> neighbors;
    neighbors.reserve(k * k);

    for (int ii = -k; ii < k + 1; ++ii)
        for (int jj = -k; jj < k + 1; ++jj)
        {
            int ki = i + ii;
            int kj = j + jj;
            if (inside(ki, kj) == true)
            {
                weight_t w = 1.0;
                w += slope_transfer(slope(ki, kj));
                // std::cout << ki << ", " << kj << ", " << w << std::endl;

                neighbors.emplace_back(index(ki, kj), w);
            }
        }
    // neighbors.shrink_to_fit();
    return neighbors;
}

adjacency_list_t Road::build_adjacency_graph() const
{
    adjacency_list_t graph;
    graph.reserve(nx * ny);
    for (int i = 0; i < nx; ++i)
        for (int j = 0; j < ny; ++j)
            graph.push_back(connexity(i, j));
    // std::cout << i << ", " << j << ", " << graph.back().size() << std::endl;
    // for (neighbor i : graph.back())
    // {
    //     auto t = inverseIndex(i.target);

    //     std::cout << t.first << ", " << t.second << i.weight << std::endl;
    // }

    return graph;
}

std::list<vertex_t> Road::compute(std::pair<int, int> source, std::pair<int, int> dest) const
{
    vertex_t src = index(source.first, source.second);
    adjacency_list_t adj = build_adjacency_graph();
    std::vector<weight_t> min_distance;
    std::vector<vertex_t> previous;
    DijkstraComputePaths(src, adj, min_distance, previous);

    vertex_t dst = index(dest.first, dest.second);

    return DijkstraGetShortestPathTo(dst, previous);
}

void Road::drawLine(
    std::vector<QVector3D> &colors,
    std::list<vertex_t> path) const
{
    std::cout << "TAILLE DE PATH : " << path.size() << std::endl;
    for (vertex_t v : path)
    {
        std::pair<int, int> p = inverseIndex(v);
        int i = p.first;
        int j = p.second;

        /*
         * j + i*nx * 2 * 3 (2 pour le nombre de triangle par carre et 3 pour le nombre de sommet par triangle)
         * 2 * 3 = 6
         * on a 6 couleurs, donc
         * 0 = indice_depart
         * 1 = indice_depart + 1
         * ...
         * 5 = indice_depart + 5
         */
        int indice_depart = (j + i * nx) * 6;
        std::cout << "D " << i << ", " << j << ", " << indice_depart;
        colors[indice_depart] = QVector3D(1., 0., 0.);
        colors[indice_depart + 1] = QVector3D(1., 0., 0.);
        colors[indice_depart + 2] = QVector3D(1., 0., 0.);
        colors[indice_depart + 3] = QVector3D(1., 0., 0.);
        colors[indice_depart + 4] = QVector3D(1., 0., 0.);
        colors[indice_depart + 5] = QVector3D(1., 0., 0.);
        std::cout << " F" << std::endl;
    }
    std::cout << "fin draw line " << std::endl;
}

///////////////////////////

void Road::DijkstraComputePaths(vertex_t source,
                                const adjacency_list_t &adjacency_list,
                                std::vector<weight_t> &min_distance,
                                std::vector<vertex_t> &previous) const
{
    int n = adjacency_list.size();
    min_distance.clear();
    min_distance.resize(n, max_weight);
    min_distance[source] = 0;
    previous.clear();
    previous.resize(n, -1);
    std::set<std::pair<weight_t, vertex_t>> vertex_queue;
    vertex_queue.insert(std::make_pair(min_distance[source], source));

    while (!vertex_queue.empty())
    {
        weight_t dist = vertex_queue.begin()->first;
        vertex_t u = vertex_queue.begin()->second;
        vertex_queue.erase(vertex_queue.begin());

        // Visit each edge exiting u
        const std::vector<neighbor> &neighbors = adjacency_list[u];
        for (std::vector<neighbor>::const_iterator neighbor_iter = neighbors.begin();
             neighbor_iter != neighbors.end();
             neighbor_iter++)
        {
            vertex_t v = neighbor_iter->target;
            weight_t weight = neighbor_iter->weight;
            weight_t distance_through_u = dist + weight;
            if (distance_through_u < min_distance[v])
            {
                vertex_queue.erase(std::make_pair(min_distance[v], v));

                min_distance[v] = distance_through_u;
                previous[v] = u;
                vertex_queue.insert(std::make_pair(min_distance[v], v));
            }
        }
    }
}

std::list<vertex_t> Road::DijkstraGetShortestPathTo(
    vertex_t vertex, const std::vector<vertex_t> &previous) const
{
    std::list<vertex_t> path;
    for (; vertex != -1; vertex = previous[vertex])
        path.push_front(vertex);
    return path;
}

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
