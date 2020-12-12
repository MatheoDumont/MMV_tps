#ifndef HEIGHT_FIELD_H
#define HEIGHT_FIELD_H

#include "scalar_field.h"
#include <vector>

#include <QImage>
#include <QVector3D>

struct Point
{
    int i, j;
    double height;

    Point() {}
    Point(int _i, int _j) : i(_i), j(_j), height(0.0) {}
    Point(int _i, int _j, double _height) : i(_i), j(_j), height(_height) {}

    bool operator<(const Point &p) const
    {
        return height < p.height;
    }
};

struct StreamAreaCell
{
    Point points[8];
    // slope[i] point i
    float slopes[8];
    // sum(slopes);
    float sum_slope = 0.f;
    // nb of object in each array
    int n = 0;

    StreamAreaCell() {}
};

class HeightField : public SF
{
public:
    enum StreamAreaFunc
    {
        D8,
        Steepest,
    };

    double minHeight;
    double maxHeight;

    HeightField();
    HeightField(const SF &s);
    HeightField(const QImage &image, const Box2D &box, double boundmin, double boundmax);
    HeightField(const HeightField &hf);

    void setMinMaxBounds();

    double height(int i, int j) const;
    double slope(int i, int j) const;
    double averageSlope(int i, int j) const;

    vec3 vertex(int i, int j) const;
    vec3 normal(int i, int j) const;

    QImage grayscale() const;
    QImage colorHSV(int rangemin, int rangemax) const;
    QImage color() const;
    QImage shade() const;

    void exportObj() const;

    SF slopeMap() const;
    SF laplacianMap() const;

    /*
     * Couleur du triangle courant construitent, necessaire pour mettre a jour 
     * le vbo et comme tonton titi dit : "pas de copier/coller, c'est haram."
     */
    void colorCell(int i, int j, std::vector<QVector3D> &colors) const;
    void getMesh(std::vector<QVector3D> &vertices,
                 std::vector<QVector3D> &colors,
                 std::vector<QVector3D> &normals) const;

    std::vector<Point> getPoints() const;

    StreamAreaCell d8(const Point &p) const;
    StreamAreaCell steepest(const Point &p) const;

    /*
     * StreamArea
     * int function, heuristique pour la selection de voisin vers qui l'eau coule :
     * 0 : D8, l'eau coule de facon pondere en fonction des pentes
     * 1 : steepest, l'eau coule a 100% vers le plus bas
     */
    SF drainage(StreamAreaFunc function) const;
};

#endif
