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
public :
    double minHeight;
    double maxHeight;
    
    HeightField();
    HeightField(const SF &s);
    HeightField(const QImage &image, const Box2D &box, double boundmin, double boundmax);
    
    double height(int i, int j) const;
    double slope(int i, int j) const;
    double averageSlope(int i, int j) const;

    vec3 vertex(int i, int j) const;
    vec3 normal(int i, int j) const;

    QImage grayscale() const;
    QImage shade() const;
    void exportObj() const;

    SF slopeMap() const;
    SF laplacianMap() const;

    std::vector<Point> getPoints() const;
    
    void getMesh(std::vector<QVector3D>& vertices,
                 std::vector<QVector3D>& colors,
                 std::vector<QVector3D>& normals) const;
    
    StreamAreaCell D8(const Point &p) const;
    StreamAreaCell steepest(const Point &p) const;

    /*
     * StreamArea
     * int function, heuristique pour la selection de voisin vers qui l'eau coule :
     * 0 : D8, l'eau coule de facon pondere en fonction des pentes
     * 1 : steepest, l'eau coule a 100% vers le plus bas
     */
    SF drainage(int function) const;
};

#endif
