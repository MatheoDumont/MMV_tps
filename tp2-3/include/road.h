#ifndef ROAD_H
#define ROAD_H
// https://perso.liris.cnrs.fr/eric.galin/Articles/2010-roads.pdf
#include "height_field.h"
#include <iostream>
#include <vector>
#include <string>
#include <list>

#include <limits> // for numeric_limits

#include <queue>
#include <utility> // for pair
#include <algorithm>
#include <iterator>
#include <set>

typedef int vertex_t;
typedef double weight_t;

struct neighbor
{
    vertex_t target;
    weight_t weight;

    neighbor() {}
    neighbor(vertex_t arg_target, weight_t arg_weight)
        : target(arg_target), weight(arg_weight) {}

    neighbor(const neighbor &n) : target(n.target), weight(n.weight) {}

    neighbor &operator=(const neighbor &rhs);
};
typedef std::vector<std::vector<neighbor>> adjacency_list_t;

class Road : public HeightField
{
private:
    // https://perso.liris.cnrs.fr/eric.galin/Articles/2010-roads.pdf#section.5
    // k la constante dans [1, ...] qui permet de dire combien de voisin on compte
    // dans le voisinage pour calculer le shortest path
    int k;
    double slope_coef;
    Grid grid;

public:
    Road(const HeightField &hf, int k, double slope_coef);
    // ~Road();

    double slope_transfer(double s) const;
    double curvature_transfer(double s) const;

    /*
     * Connexity => fonction pour obtenir tous les voisins d'un noeud depuis :
     *  - Les voisins de la surface
     *  - Les voisins pour les ponts
     *  - Les voisins pour les tunnels
     * et charge la liste d'adjacence avec les poids calcules :
     *  f o t(p, p`, p``) avec f une fonction de transfere,
     *   p la position du voisin, p` sa pente et p`` sa courbure.
     */
    std::vector<neighbor> connexity(int i, int j) const;

    adjacency_list_t build_adjacency_graph() const;

    std::list<vertex_t> compute(const std::pair<int, int> &source, const std::pair<int, int> &dest) const;

    bool drawLine(std::vector<QVector3D> &colors,
                  std::list<vertex_t> path) const;
    void colorSquare(int i, int j, std::vector<QVector3D> &colors, const QVector3D &color) const;

    // https://rosettacode.org/wiki/Dijkstra%27s_algorithm#C.2B.2B
    void DijkstraComputePaths(vertex_t source,
                              const adjacency_list_t &adjacency_list,
                              std::vector<weight_t> &min_distance,
                              std::vector<vertex_t> &previous) const;

    std::list<vertex_t> DijkstraGetShortestPathTo(
        vertex_t vertex, const std::vector<vertex_t> &previous) const;
};

#endif
