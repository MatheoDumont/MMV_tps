#ifndef ROAD_H
#define ROAD_H

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

const weight_t max_weight = std::numeric_limits<double>::infinity();

struct neighbor
{
    vertex_t target;
    weight_t weight;
    neighbor(vertex_t arg_target, weight_t arg_weight)
        : target(arg_target), weight(arg_weight) {}
};

typedef std::vector<std::vector<neighbor>> adjacency_list_t;
typedef std::pair<weight_t, vertex_t> weight_vertex_pair_t;

class Road
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
    std::vector<neighbor> connexity();

public:
    Road(/* args */);
    // ~Road();
};

#endif