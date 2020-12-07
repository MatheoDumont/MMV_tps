#ifndef HEIGHT_FIELD_H
#define HEIGHT_FIELD_H

#include "scalar_field.h"
#include <vector>

#include <QImage>
#include <QVector3D>

struct Point {
    int i, j;
    double height;

    Point(int _i, int _j, double _height) : i(_i), j(_j), height(_height) {}

    bool operator<(const Point &p) const
    {
        return height < p.height;
    }
};

class HeightField : public SF
{
public :
    double minHeight;
    double maxHeight;
    
    HeightField();
    HeightField(const SF& s);
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
    
    SF drainage() const;
};

#endif
