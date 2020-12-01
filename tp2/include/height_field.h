#ifndef HEIGHT_FIELD_H
#define HEIGHT_FIELD_H

#include "scalar_field.h"

#include <QImage>

class HeightField : public SF
{
public :
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
    SF drainage() const;
};

#endif
