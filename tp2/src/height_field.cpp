#include "height_field.h"

HeightField::HeightField(const SF2 &s) : SF2(s) {}

// HeightField::HeightField(const QImage &image, const Box2D &box,
//                          double boundmin, double boundmax) : Grid(box, image.width(), image.height())
// {
//     // Interpoler linéaire entre les 2 z (bound min et max correspondent respectivement
//     // a la couleur min et la couleur max(blanc et noir))
// }

HeightField::HeightField(const QImage &image, const Grid &g,
                         double boundmin, double boundmax) : SF2(g)
{
    // Interpoler linéaire entre les 2 z (bound min et max correspondent respectivement
    // a la couleur min et la couleur max(blanc et noir))

    int delta_height = floor(image.height() / ny);
    int delta_width = floor(image.width() / nx);

    for (int i = 0, y = 0; y < image.height(); y += delta_height, i++)
        for (int j = 0, x = 0; x < image.width(); x += delta_width, j++)
            field[index(i, j)] = normalization(image.pixel(x, y) / 255., boundmin, boundmax);
}

double HeightField::height(int i, int j) const
{
    return at(i, j);
}

double HeightField::slope(int i, int j) const
{
    vec2 g = gradient(i, j);
    return sqrt(dot(g, g));
}

/** x x x                   x       x       x
 *  x o x   Moyenne |o-x| +  |o| + |o| + |o|   ...
 *  x x x   permet d'avoir des pentes élevées sur les pics mais plus de direction associée
 */
double HeightField::averageSlope(int i, int j) const
{
    return 0.0; // pour compiler TODO
}

vec3 HeightField::vertex(int i, int j) const
{
    vec2 v = Grid::vertex(i, j);
    return vec3(v.x, height(i, j), v.y); // vec3(x, z, y);
}

vec3 HeightField::normal(int i, int j) const
{
    vec2 g = gradient(i, j);

    return normalize(vec3(-g.x, -g.y, 1.0));
    // return normalize(vec3(-g.x, 1.0, -g.y));
}

QImage HeightField::greyscale() const
{
    QImage image(nx, ny, QImage::Format_Grayscale8);
    // https://en.cppreference.com/w/cpp/algorithm/minmax_element
    const auto [min, max] = std::minmax_element(field.begin(), field.end());

    for (int y = 0; y < ny; y++)
        for (int x = 0; x < nx; x++)
            image.setPixel(y, x, normalization(height(y, x) / *max, 0, 255));

    return image;
}

QImage HeightField::shade() const
{
    // TODO: A modifier
    QImage image(nx, ny, QImage::Format_ARGB32);
    const vec3 lightdir = normalize(vec3(1.0, 1.0, 3.0)); // changer Z UP il a dit donc t'avait raison

    // Relief shading: vec3 lightdir
    for (int i = 0; i < nx; ++i)
    {
        for (int j = 0; j < ny; ++j)
        {
            // Faire du Phong (ambiant + diffu où d = lightdir * normal)
            vec3 n = this->normal(i, j);
            double d = dot(lightdir, n); // d in [-1, 1]
            d = (1.0 + d) * 0.5;         // d in [0, 1]
            d = d * d;
            // en haut de bosse laplcien tres negatif, tres positif = plus profond d'un creux , 0 = etendue plate

            // Altérer la couleur en fonction de l'altitude
            // or height(i, j) est l'altitude
            // bon site relief shading passionnant tous les gens l'aiment clique pouce bleu partage abonne toi (reliefshading.com)
            // encore mieux "shadedrelief.com"
            // image.setPixel(i, j, ...); // donner light dir -> gagner + un shader de machin : ambiennte + diffuse
        }
    }
    return image;
}

SF2 HeightField::slopeMap() const
{
    SF2 s(Grid(Box2D(origin, length), nx, ny));

    for (int i = 0; i < nx; i++)
    {
        for (int j = 0; j < ny; j++)
        {
            s.at(i, j) = slope(i, j); // On é tro forr
        }
    }

    return s;
}

SF2 HeightField::drainage() const
{
    // On va se casser les dents dessus
    // pro tips : on attend la correction +1
    // calcul le drainage, algorithme de calcul d'aire de je sais pas quoi (pas sur)
    // le drainage :
    // 3 etapes :
    // 1. trier tous les points Pij selon Zij, leurs hauteur -> tableau T (trié)
    //      trie => std::sort (il faut une structure pour le faire selon z)
    // 2. initialiser un SF2 a (aire de drainage) de la taile qui va bien (nx, ny) avec 1.0 dedans pour chaque point.
    // 3. pour tous les points de T dans l'ordre descendant (du plus haut au plus bas)
    // -> modifier la valeur de drainage comme :
    //      Point Qij = valeur
    //      voir vers qui la cellule Qij coule => vers les points qui sont plus bas(8-adjacence) appellons Qi'j'
    //      double Ai'j' = quantite d'eau dans qui coule de Qij vers la cellule Qi'j'
    //      y faire couler une fraction de Qij vers Qi'j'
    //      et ca pour tous les voisins eligible (plus bas que Qij) /!\ : Aij ne change pas (à revérifier quand même)
    //      Trouver les v voisins (0 <= v <= 8) et calculer la fraction d'eau qui se barre vers chacun d'creux
    //      heuristique :
    //        * l'eau coule a 100% vers le plus bas
    //        * l'eau coule de facon pondere en fonction des pentes
    //        * 2 algo a voir : le steepest et l'algo D8
    //              steapest (100% du déversement à la cellule la plus basse, ie. ayant la plus forte pente)
    //              D8 (on lance un dé à 8 faces) X)
    // Mais si on laisse l'eau dans les Qij et dans Qi`j`, on aura des valeurs partout moyenne et des grandes valeurs dans les trous
    // em plus, ca veut dire qu'on cree de l'eau
}
