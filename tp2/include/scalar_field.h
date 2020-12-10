#ifndef SCALAR_FIELD_H
#define SCALAR_FIELD_H

#include "grid.h"

#include <vector>
#include <QImage>

class SF : public Grid
{
protected:
    std::vector<double> field;

public:
    SF();
    SF(const Grid &g);
    SF(const SF &sf);

    double at(int i, int j) const;
    double &at(int i, int j);

    /**
     * Dérivée première
     *
     * Propriété simple on peut l'approximer avec :
     *   df/dx, df/dy ~ ((f(x + epsilon, y) - f(x - epsilon, y)) / 2epsilon, ...pareil pour y )
     */
    vec2 gradient(int i, int j) const;

    /**
     * Dérivée seconde
     *
     * Propriété simple on peut l'approximer avec :
     *   d2f / dx2 ~ (f(x + epsilon) - 2f(x) + f(x + epsilon)) / (epsilon * epsilon)
     *
     * (https://fr.wikipedia.org/wiki/Laplacien_discret)
     */
    double laplacian(int i, int j) const;

    double normalization(double x, double in_min, double in_max, double out_min, double out_max) const;

    /**
     * [in_min, in_max] = [0, 1]
     */
    double normalization(double x, double out_min, double out_max) const;
    double clamp(double min, double max, double x) const;
    vec2 clamp(const vec2 &) const;

    /*
     * Applique un filtre 2D de taille 3*3 a field.
     * filter_choice : 
     * 0 : filtre moyenneur => smooth
     * 1 : filtre gaussien => blur
     * Les masques sont hard codes.
     */
    void filter(int filter_choice);

};

#endif
