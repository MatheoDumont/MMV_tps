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

static const QVector3D BLACK = QVector3D(0.0, 0.0, 0.0);
static const QVector3D WHITE = QVector3D(1.0, 1.0, 1.0);
static const QVector3D RED = QVector3D(1.0, 0.0, 0.0);
static const QVector3D LIME = QVector3D(0.0, 1.0, 0.0);
static const QVector3D BLUE = QVector3D(0.0, 0.0, 1.0);
static const QVector3D YELLOW = QVector3D(1.0, 1.0, 0.0);
static const QVector3D CYAN = QVector3D(0.0, 1.0, 1.0);
static const QVector3D MAGENTA = QVector3D(1.0, 0.0, 1.0);
static const QVector3D SILVER = QVector3D(0.75, 0.75, 0.75);
static const QVector3D GRAY = QVector3D(0.5, 0.5, 0.5);
static const QVector3D MAROON = QVector3D(0.5, 0.0, 0.0);
static const QVector3D OLIVE = QVector3D(0.5, 0.5, 0.0);
static const QVector3D GREEN = QVector3D(0.0, 0.5, 0.0);
static const QVector3D PURPLE = QVector3D(0.5, 0.0, 0.5);
static const QVector3D TEAL = QVector3D(0.0, 0.5, 0.5);
static const QVector3D NAVY = QVector3D(0.0, 0.0, 0.5);

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

Road::Road(const HeightField &hf, int _k, double _slope_coef)
    : HeightField(hf), k(_k), slope_coef(_slope_coef)
{
    grid = Grid((Box2D)hf, hf.getNX() - 1, hf.getNY() - 1);
}

double Road::slope_transfer(double s) const
{
    return s * slope_coef;
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
            if (grid.inside(ki, kj) == true)
            {
                weight_t w = 1.0;
                w += slope_transfer(slope(ki, kj));

                neighbors.emplace_back(grid.index(ki, kj), w);
            }
        }
    return neighbors;
}

adjacency_list_t Road::build_adjacency_graph() const
{
    adjacency_list_t graph;
    graph.reserve(grid.getNX() * grid.getNY());
    for (int i = 0; i < grid.getNX(); ++i)
        for (int j = 0; j < grid.getNY(); ++j)
            graph.push_back(connexity(i, j));

    return graph;
}

std::list<vertex_t> Road::compute(const std::pair<int, int> &source, const std::pair<int, int> &dest) const
{
    int x, y, nx, ny;

    nx = grid.getNX();
    ny = grid.getNY();

    x = source.first;
    if (x == nx)
        x = nx - 1;
    else if (x == nx - 1)
        x = nx - 2;

    y = source.second;
    if (y == ny)
        y = ny - 1;
    else if (y == ny - 1)
        y = ny - 2;

    vertex_t src = grid.index(x, y);

    adjacency_list_t adj = build_adjacency_graph();
    std::vector<weight_t> min_distance;
    std::vector<vertex_t> previous;
    DijkstraComputePaths(src, adj, min_distance, previous);

    x = dest.first;
    if (x == nx)
        x = nx - 1;
    else if (x == nx - 1)
        x = nx - 2;

    y = dest.second;
    if (y == ny)
        y = ny - 1;
    else if (y == ny - 1)
        y = ny - 2;

    vertex_t dst = grid.index(x, y);

    return DijkstraGetShortestPathTo(dst, previous);
}

#define ITER_COLOR(I, J, COLOR)                                 \
  for (istep = -1; istep < 2; ++istep)                          \
      for (jstep = -1; jstep < 2; ++jstep)                      \
          if (grid.inside(I + istep, J + jstep) == true)        \
              colorSquare(I + istep, J + jstep, colors, COLOR);

bool Road::drawLine(
    std::vector<QVector3D> &colors,
    std::list<vertex_t> path) const
{
    int istep, jstep;
    std::pair<int, int> p;

    if (path.size() == 1)
        return false;

    vertex_t f = path.front();
    path.pop_front();

    p = grid.inverseIndex(f);

    ITER_COLOR(p.first, p.second, GREEN);
    //colorSquare(p.first, p.second, colors, GREEN);

    for (vertex_t v : path)
    {
        p = grid.inverseIndex(v);
        ITER_COLOR(p.first, p.second, RED);
    }

    ITER_COLOR(p.first, p.second, BLUE);

    return true;
}

void Road::colorSquare(int i, int j, std::vector<QVector3D> &colors, const QVector3D &color) const
{
    /*
    * j + i*ny * 2 * 3 (2 pour le nombre de triangle par carre et 3 pour le nombre de sommet par triangle)
    * 2 * 3 = 6
    * on a 6 couleurs, donc
    * 0 = indice_depart
    * 1 = indice_depart + 1
    * ...
    * 5 = indice_depart + 5
    */
    int indice_depart = grid.index(i, j) * 6;
    for (int i = 0; i < 6; ++i)
        colors[indice_depart + i] = color;
}

/***************** ROSETTA CODE *****************/
// https://rosettacode.org/wiki/Dijkstra%27s_algorithm#C.2B.2B

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
