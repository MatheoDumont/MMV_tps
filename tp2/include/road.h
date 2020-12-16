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

typedef int vertex_t;
typedef double weight_t;

struct neighbor
{
    vertex_t target;
    weight_t weight;

    neighbor(vertex_t arg_target, weight_t arg_weight)
        : target(arg_target), weight(arg_weight) {}
};

const double max_slope = 50.0;
class Road : public HeightField
{
private:
    // https://perso.liris.cnrs.fr/eric.galin/Articles/2010-roads.pdf#section.5
    // k la constante dans [1, ...] qui permet de dire combien de voisin on compte
    // dans le voisinage pour calculer le shortest path
    int k;

protected:
    double slope_transfer(double s);
    double curvature_transfer(double s);

    /*
     * Connexity => fonction pour obtenir tous les voisins d'un noeud depuis :
     *  - Les voisins de la surface
     *  - Les voisins pour les ponts
     *  - Les voisins pour les tunnels
     * et charge la liste d'adjacence avec les poids calcules :
     *  f o t(p, p`, p``) avec f une fonction de transfere,
     *   p la position du voisin, p` sa pente et p`` sa courbure.
     */
    std::vector<neighbor> connexity(vertex_t u);

    // https://rosettacode.org/wiki/Dijkstra%27s_algorithm#C.2B.2B
    void DijkstraComputePaths(vertex_t source,
                              std::vector<weight_t> &min_distance,
                              std::vector<vertex_t> &previous);

    std::list<vertex_t> DijkstraGetShortestPathTo(
        vertex_t vertex, const std::vector<vertex_t> &previous);

    std::list<std::pair<int, int>> compute(vertex_t source);

public:
    Road(const HeightField &hf, int k);
    // ~Road();
};

#endif