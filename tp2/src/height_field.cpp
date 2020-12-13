#include "height_field.h"

#include <algorithm>
#include <cmath>
#include <QImage>
#include <QColor>

HeightField::HeightField() : SF() {}

HeightField::HeightField(const SF &s) : SF(s)
{
    setMinMaxBounds();
}

HeightField::HeightField(const QImage &image, const Box2D &box,
                         double boundmin, double boundmax)
    : SF(Grid(box, image.width(), image.height())),
      minHeight(boundmin), maxHeight(boundmax)
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
    for (int i = 0; i < image.width(); ++i)
        for (int j = 0; j < image.height(); ++j)
            field[index(i, j)] = normalization(qGray(grayscale.pixel(i, j)),
                                               0.0, 255.0,
                                               boundmin, boundmax);
}

HeightField::HeightField(const HeightField &hf)
    : SF(hf), minHeight(hf.minHeight), maxHeight(hf.maxHeight) {}

HeightField &HeightField::operator=(const HeightField &rhs)
{
    if (this != &rhs)
    {
        a = rhs.a;
        b = rhs.b;
        diagonal = rhs.diagonal;

        nx = rhs.nx;
        ny = rhs.ny;
        celldiagonal = rhs.celldiagonal;
        inversecelldiagonal = rhs.inversecelldiagonal;

        field = rhs.field;

        minHeight = rhs.minHeight;
        maxHeight = rhs.maxHeight;
    }

    return *this;
}

void HeightField::setMinMaxBounds()
{
    const auto pair = std::minmax_element(field.begin(), field.end());
    minHeight = *pair.first;
    maxHeight = *pair.second;
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
    int i, j;
    vec3 v;
    QColor color;
    QImage image(nx, ny, QImage::Format_Grayscale8);

    for (i = 0; i < nx; ++i)
        for (j = 0; j < ny; ++j)
        {
            v = getGrayscale(i, j, 0, 255);
            color = QColor(v.r(), v.g(), v.b());
            image.setPixelColor(i, j, color);
        }

    return image;
}

QImage HeightField::colorHSV(int rangemin, int rangemax) const
{
    int i, j;
    vec3 v;
    QColor color;
    QImage image(nx, ny, QImage::Format_RGB16);

    for (i = 0; i < nx; ++i)
        for (j = 0; j < ny; ++j)
        {
            v = getColorHSV(i, j, 0, 255, rangemin, rangemax);
            color = QColor(v.r(), v.g(), v.b());
            image.setPixelColor(i, j, color);
        }

    return image;
}

QImage HeightField::color() const
{
    int i, j;
    vec3 v;
    QColor color;
    QImage image(nx, ny, QImage::Format_RGB16);

    for (i = 0; i < nx; ++i)
        for (j = 0; j < ny; ++j)
        {
            v = getColor(i, j, 0, 255);

            color = QColor(v.r(), v.g(), v.b());
            image.setPixelColor(i, j, color);
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

vec3 HeightField::getGrayscale(int i, int j, double min, double max) const
{
    double val;

    val = normalization(height(i, j), minHeight, maxHeight, min, max);

    return vec3(val, val, val);
}

vec3 HeightField::getColorHSV(int i, int j, double min, double max, int rangemin, int rangemax) const
{
    int h, s, v;

    h = (int)normalization(height(i, j), minHeight, maxHeight, rangemin, rangemax);
    // s = (int)normalization(height(i, j), minHeight, maxHeight, 0.0, 255.0);
    v = (int)normalization(height(i, j), minHeight, maxHeight, 0.0, 255.0);

    QColor color = QColor::fromHsv(h, 128, v);

    if (max == 1.0)
        return vec3(color); // Normalisé entre 0.0 et 1.0;

    return vec3(color.red(), color.green(), color.blue()); // Normalisé entre 0 et 255
}

vec3 HeightField::getColor(int i, int j, double min, double max) const
{
    double v;

    v = normalization(std::pow(height(i, j), 1.8), minHeight, maxHeight, min, max);

    return vec3(v, 0.0, max - v);
}

void HeightField::vertexCell(int i, int j, std::vector<QVector3D> &vertices) const
{
    vec3 v0, v1, v2, v3;
    v0 = vertex(i, j);
    v1 = vertex(i + 1, j);
    v2 = vertex(i + 1, j + 1);
    v3 = vertex(i, j + 1);

    vertices.emplace_back(v0.x, v0.y, v0.z);
    vertices.emplace_back(v1.x, v1.y, v1.z);
    vertices.emplace_back(v2.x, v2.y, v2.z);

    vertices.emplace_back(v0.x, v0.y, v0.z);
    vertices.emplace_back(v2.x, v2.y, v2.z);
    vertices.emplace_back(v3.x, v3.y, v3.z);
}

void HeightField::colorCell(int i, int j, std::vector<QVector3D> &colors,
                            ColorType type, int rangemin, int rangemax) const
{
    vec3 v0, v1, v2, v3;

    switch (type)
    {
    case Grayscale:
        v0 = getGrayscale(i, j, 0.0, 1.0);
        v1 = getGrayscale(i + 1, j, 0.0, 1.0);
        v2 = getGrayscale(i + 1, j + 1, 0.0, 1.0);
        v3 = getGrayscale(i, j + 1, 0.0, 1.0);
        break;

    case HSV:
        v0 = getColorHSV(i, j, 0.0, 1.0, rangemin, rangemax);
        v1 = getColorHSV(i + 1, j, 0.0, 1.0, rangemin, rangemax);
        v2 = getColorHSV(i + 1, j + 1, 0.0, 1.0, rangemin, rangemax);
        v3 = getColorHSV(i, j + 1, 0.0, 1.0, rangemin, rangemax);
        break;

    case Coloring:
        v0 = getColor(i, j, 0.0, 1.0);
        v1 = getColor(i + 1, j, 0.0, 1.0);
        v2 = getColor(i + 1, j + 1, 0.0, 1.0);
        v3 = getColor(i, j + 1, 0.0, 1.0);
        break;

    default:
        break;
    }

    colors.emplace_back(v0.x, v0.y, v0.z);
    colors.emplace_back(v1.x, v1.y, v1.z);
    colors.emplace_back(v2.x, v2.y, v2.z);

    colors.emplace_back(v0.x, v0.y, v0.z);
    colors.emplace_back(v2.x, v2.y, v2.z);
    colors.emplace_back(v3.x, v3.y, v3.z);
}

void HeightField::normalCell(int i, int j, std::vector<QVector3D> &normals) const
{
    vec3 v0, v1, v2, v3;
    v0 = normal(i, j);
    v1 = normal(i + 1, j);
    v2 = normal(i + 1, j + 1);
    v3 = normal(i, j + 1);

    normals.emplace_back(v0.x, v0.y, v0.z);
    normals.emplace_back(v1.x, v1.y, v1.z);
    normals.emplace_back(v2.x, v2.y, v2.z);

    normals.emplace_back(v0.x, v0.y, v0.z);
    normals.emplace_back(v2.x, v2.y, v2.z);
    normals.emplace_back(v3.x, v3.y, v3.z);
}

void HeightField::getMesh(double &maxHeight,
                          std::vector<QVector3D> &vertices,
                          std::vector<QVector3D> &colors,
                          std::vector<QVector3D> &normals) const
{
    int i, j;

    for (i = 0; i < nx - 1; ++i)
        for (j = 0; j < ny - 1; ++j)
        {
            // Vertices
            vertexCell(i, j, vertices);

            // Colors
            colorCell(i, j, colors, ColorType::Grayscale);

            // Normals
            normalCell(i, j, normals);
        }

    maxHeight = this->maxHeight;
}

std::vector<Point> HeightField::getPoints() const
{
    std::vector<Point> points;
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; ++j)
            points.push_back(Point(i, j, height(i, j)));

    return points;
}

std::pair<int, int> next[8] = {{1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}};
const float sqrt_2 = 1.41421356237;
float distance[8] = {1., sqrt_2, 1., sqrt_2, 1., sqrt_2, 1., sqrt_2};

StreamAreaCell HeightField::d8(const Point &p) const
{
    // float diag = celldiagonal.length();
    // float d[8] = {celldiagonal.x, diag, celldiagonal.y, diag, celldiagonal.x, diag, celldiagonal.y, diag};

    struct StreamAreaCell cell;
    for (int k = 0; k < 8; ++k)
    {
        int i = p.i + next[k].first;
        int j = p.j + next[k].second;

        if (inside(i, j))
        {
            double diff_height = p.height - height(i, j);
            if (diff_height > 0.0)
            {
                cell.slopes[cell.n] = diff_height / distance[k];
                cell.sum_slope += cell.slopes[cell.n];
                cell.points[cell.n] = Point(i, j);
                cell.n++;
            }
        }
    }
    return cell;
}

static inline int argmax(double d1, double d2) { return d1 > d2 ? 0 : 1; }

StreamAreaCell HeightField::steepest(const Point &p) const
{
    struct StreamAreaCell cell;
    double max_slope = -1000.0;
    Point max_point;
    bool creux = true;
    for (int k = 0; k < 8; ++k)
    {
        int i = p.i + next[k].first;
        int j = p.j + next[k].second;

        if (inside(i, j))
        {
            double diff_height = p.height - height(i, j);
            if (diff_height > 0.0)
            {
                creux = false;
                double slope = diff_height / distance[k];
                if (argmax(max_slope, slope) == 1)
                {
                    max_slope = slope;
                    max_point = Point(i, j);
                }
            }
        }
    }

    if (creux)
    {
        cell.n = 0;
        return cell;
    }

    cell.points[0] = max_point;
    cell.slopes[0] = max_slope;
    cell.sum_slope = max_slope;
    cell.n = 1;
    return cell;
}

SF HeightField::streamArea(StreamAreaFunc function) const
{
    // 3 etapes :
    // 1. trier tous les points Pij selon Zij, leurs hauteur -> tableau T (trié)
    //      trie => std::sort (il faut une structure pour le faire selon z)
    std::vector<Point> points = getPoints();
    std::sort(points.begin(), points.end());

    // 2. initialiser un SF a (aire de drainage) de la taile qui va bien (nx, ny) avec 1.0 dedans pour chaque point.
    SF sf = SF(*this);
    for (int i = 0; i < nx; ++i)
        for (int j = 0; j < ny; ++j)
            sf.at(i, j) = 1.0;

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
    if (function == D8)
    {
        for (Point p : points)
        {
            StreamAreaCell cell = d8(p);

            for (int i = 0; i < cell.n; ++i)
                sf.at(cell.points[i].i, cell.points[i].j) += (sf.at(p.i, p.j) * cell.slopes[i]) / cell.sum_slope;
        }
    }
    else if (function == Steepest)
    {
        for (Point p : points)
        {
            StreamAreaCell cell = steepest(p);

            for (int i = 0; i < cell.n; ++i)
                sf.at(cell.points[i].i, cell.points[i].j) += (sf.at(p.i, p.j) * cell.slopes[i]) / cell.sum_slope;
        }
    }

    return sf;
}

SF HeightField::streamPower() const
{

    SF stream = streamArea(D8);
    SF slope = slopeMap();
    SF result(Grid(*this));

    for (int i = 0; i < nx - 1; ++i)
        for (int j = 0; j < ny - 1; ++j)
            result.at(i, j) = sqrt(stream.at(i, j)) * slope.at(i, j);

    return result;
}

SF HeightField::wetnessIndex() const
{
    // ln(𝐴𝐴)/(𝑠𝑠+𝜀𝜀)
    double epsilon = 0.0001;
    SF stream = streamArea(D8);
    SF slope = slopeMap();
    SF result(Grid(*this));
    for (int i = 0; i < nx - 1; ++i)
        for (int j = 0; j < ny - 1; ++j)
            result.at(i, j) = std::log(stream.at(i, j)) / (slope.at(i, j) + epsilon);

    return result;
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
//             // lengsteepestth tableau de taille 8 de length des cell dans la 8-adjacence = [1, sqrt(2), 1, sqrt(2), 1, sqrt(2), 1, sqrt(2)]
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
...
je vous rappelle que ici on travail avec une surface d'elevation z = h(x, y)
f(x,y,z) = z - h(x,y)
distance bound slide 21 implicit modeling
µ = sqrt(1 + lambda*lambda)

si on a un terrain definit par une grille d'elevation, qu'elle est la borne de ce machin la:
on a plus qu'a trouver la fonction mu et c'est gagner => bonne question de fdp RIP nous

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
*/
