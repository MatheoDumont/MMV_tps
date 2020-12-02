#include "height_field.h"

#include <algorithm>
#include <cmath>
#include <QImage>

HeightField::HeightField(const SF &s) : SF(s) {}
HeightField::HeightField(const QImage &image, const Box2D &box,
                         double boundmin, double boundmax)
    : SF(Grid(box, image.width(), image.height()))
{
    // Interpoler linéaire entre les 2 z (bound min et max correspondent respectivement
    // a la couleur min et la couleur max(blanc et noir))

    // Pas besoin de delta_x et delta_y parce que la Grid est initialisé avec
    // les dimensions de l'image
    
    QImage grayscale;
    if (image.isGrayscale() != true)
        grayscale = image.convertToFormat(QImage::Format_Grayscale8);
    else
        grayscale = image;

    // Bon bah je sens qu'on va pas pouvoir être plus précis qu'un int [0, 255] ... :/
    for (int i = 0; i < image.height(); ++i)
        for (int j = 0; j < image.width(); ++j)
            field[index(i, j)] = normalization(qGray(grayscale.pixel(i, j)),
                                               0.0, 255.0,
                                               boundmin, boundmax);
}

double HeightField::height(int i, int j) const
{
    return at(i, j);
}

double HeightField::slope(int i, int j) const
{
    return gradient(i, j).length();
}

/** x x x                   x       x       x
 *  x o x   Moyenne |o-x| +|  o| + |o| + |o  | + ...
 *  x x x   permet d'avoir des pentes élevées sur les pics mais plus de direction associée
 */
double HeightField::averageSlope(int i, int j) const
{
    double avg = 0.0;
    int count = 0;
    int x, y;

    for (x = -1; x < 2; ++x)
        for (y = -1; y < 2; ++y)
            if (border(i + x, j + y) == false)
            {
                avg += abs(at(i, j) - at(i + x, j + y)); // Ou abs(slope(i, j) - slope(i + x, j + y))
                count++;
            }

    if (count != 0.0)
        avg /= double(count);

    return avg;
}

vec3 HeightField::vertex(int i, int j) const
{
    vec2 v = Grid::vertex(i, j);
    return vec3(v.x, height(i, j), v.y); // vec3(x, z, y);
}

vec3 HeightField::normal(int i, int j) const
{
    vec2 g = gradient(i, j);

    return vec3(-g, 1.0).normalize();
}

QImage HeightField::grayscale() const
{
    QImage image(nx, ny, QImage::Format_Grayscale8);
    
    // https://en.cppreference.com/w/cpp/algorithm/minmax_element
    const auto pair = std::minmax_element(field.begin(), field.end());
    const auto min = pair.first;
    const auto max = pair.second;

    double val;
    for (int i = 0; i < nx; ++i)
        for (int j = 0; j < ny; ++j)
        {
            val = normalization(height(i, j), *min, *max, 0.0, 255.0);
            image.setPixelColor(i, j, QColor(val, val, val));
        }

    return image;
}

QImage HeightField::shade() const
{
    // TODO: A modifier
    QImage image(nx, ny, QImage::Format_ARGB32);
    const vec3 lightdir = vec3(1.0, 1.0, 3.0).normalize(); // changer Z UP il a dit donc t'avait raison

    // Relief shading: vec3 lightdir
    for (int i = 0; i < nx; ++i)
    {
        for (int j = 0; j < ny; ++j)
        {
            // Faire du Phong (ambiant + diffu où d = lightdir * normal)
            vec3 n = normal(i, j);
            double d = dot(lightdir, n); // d in [-1, 1]
            d = (1.0 + d) * 0.5;         // d in [0, 1]
            d *= d;

            image.setPixelColor(i, j, QColor(d)); // Pour tester

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

SF HeightField::slopeMap() const
{
    SF s(Grid(Box2D(a, b), nx, ny));

    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++)
            s.at(i, j) = slope(i, j);

    return s;
}

SF HeightField::laplacianMap() const
{
    SF s(Grid(Box2D(a, b), nx, ny));

    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++)
            s.at(i, j) = laplacian(i, j);

    return s;
}

std::vector<Point> HeightField::getPoints() const
{
    std::vector<Point> points;
    for (int i = 0; i < nx; i++)
      for (int j = 0; j < ny; ++j)
          points.push_back(Point(i, j, height(i, j)));

    return points;
}

SF HeightField::drainage() const
{
    std::vector<Point> points = getPoints();
    std::sort(points.begin(), points.end());
    // On va se casser les dents dessus
    // pro tips : on attend la correction +1
    // calcul le drainage, algorithme de calcul d'aire de je sais pas quoi (pas sur)
    // le drainage :
    // 3 etapes :
    // 1. trier tous les points Pij selon Zij, leurs hauteur -> tableau T (trié)
    //      trie => std::sort (il faut une structure pour le faire selon z)
    // 2. initialiser un SF a (aire de drainage) de la taile qui va bien (nx, ny) avec 1.0 dedans pour chaque point.
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

    return SF(Grid(Box2D(a, b), nx, ny)); // pour compiler
}


/**
 * Code du prof pour qu'on ait la logique
 */
// int HeightField::CheckFlowSlope(const QPoint &p, QPoint *point, double *height, double *slope, double *nslope, int &mask) const
// {
//     int n = 0;
//     double zp = at(p);
//     double slopesum = 0.0;
//     mask = 0;
//     for (int i = 0; i < 8; i++)
//     {
//         QPoint b = p + Array2::next[i]; // Skip if point is not inside the domain
//         if (!InsideVertexIndex(b.x(), b.y()))
//             continue;
//
//         double step = at(b) - zp;
//         if (step < 0.0)
//         {
//             point[n] = b;
//             height[n] = -step;
//             // length tableau de taille 8 de length des cell dans la 8-adjacence = [1, sqrt(2), 1, sqrt(2), 1, sqrt(2), 1, sqrt(2)]
//             slope[n] = -step / length[i];
//             slopesum += slope[n];
//             n++;
//             mask |= 1 << i;
//         }
//     }
//     // Relative slope
//     for (int k = 0; k < n; k++)
//         nslope[k] = slope[k] / slopesum;
//
//     return n;
// }

// Tain j'ai vraiment l'impression qu'il est en train de refaire le cours de raytracing x)
/*
enfin, de ce que je m'en souviens, oui
~~~~~~~~~~~~~~~~~~~~~~~~~
--------------------------
oui alors
--------------------------
non Mais
----------------------------
triter des terrains qui font 10 km, ca fait le job, pas besoinsd e plus de precision, voila Pour
le calcul d'accessibilite
--------------------------
pas tres tres difficile, ya une seule fonciton qui peut retenir notre attention veritablement
calcul de la constante de lipschitz
et comment je calcul l'intersection et le terrain
vous avez vu quand on a fait le prmier tp avec le shahder je peux reprenseter le terrain
avec f(x, y) =0
donc je considere mon terrain comme un surface implicite, et on fait un spheretracing
donc la constante de lipschitz pour notre fonction c'est ... x) Tintintin
je m'emmeerde et ca me fait ecouter
je lai plus ...
normalement je vous avais parle de ca ... excusez moi
...
il me semblait que j'en fuck

je vous rappelle que ici on travail avec une surface d'elevation z = h(x, y)
f(x,y,z) = z - h(x,y)
distance bound slide 21 implicit modeling
µ = sqrt(1 + lambda*lambda)

si on a un terrain definit par une grille d'elevation, qu'elle est la borne de ce machin la:
on a plus qu'a trouver la fonction mu et c'est gagner => bonne question de fdp RIP nous
je vous laisse mourir
X_X
mtn il regarde son porn :smirk:
moi auss je suis perdu

f/mu c'est un distancebound et lambda lipschitz de h
f est un scalarfield qui a une seule Propriété de borne
distancebound = fonction telle que b minor la distance euclidienne
f/mu minor est bien une distancebound

on a une grille d'echantillon, comment calculer le lambda de h :
le max de toutes les diffs de pentes adjacentes
Pour toutes les pentes : max(pente) = lambda
pour chqaue pts, calculer la plus grande pente avec les 8 voisins et calculer le sup de tout ca. et c'est fini

double SF::K() const
{
  double k = 0.;

  for (int i = 0; i < nx : i++)
    for (int j = 0; j < ny; ++j)
        k = math::max(k, gradient(i,j).length());

  return k;
}

l'intersect :
on l'a dans les shader : on doit reconnaitre en gros le coeur du sphere dans le raymarching (jai la photo): Noice

quetion : souvent ce qu'on veut faire pour un terrain a une cerataine position : on veut approximer l'eclairement,
comment est ce que les differents pts du terrain sont ensoleilles au cours de l'annee.
Qu'est ce qu'il faudrait faire pour calculer l'ensoleillement moyen dans l'annee :
je sais pas
on fait des releves puis grosse moyenne


E(p) = somme sur tous les jour, de la somme sur toutes les heures de l'ensoleillement au point p,
puisque la position soleil s varie en fonction de l'heure et du jour.
s est fonction du jour, de l'heure et et et ? de la latitude et longitude.
ya des codes pour connaitre ca, et faire une jolie map
si on veut sucer on fait ca
*/
