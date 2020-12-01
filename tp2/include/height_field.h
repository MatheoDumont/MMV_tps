#ifndef HEIGHT_FIELD_H
#define HEIGHT_FIELD_H

#include "scalar_field.h"
#include "box2d.h"
#include "vec.h"
#include <QImage>
#include <cmath>
#include <QImage>

class HeightField : public SF2
{
public :
  HeightField(const SF2& s);
  HeightField(const QImage &, const Grid&, double, double);

  double height(int i, int j) const;
  double slope(int i, int j) const;
  double averageSlope(int i, int j) const;

  vec3 vertex(int i, int j) const;
  vec3 normal(int i, int j) const;

  QImage greyscale() const;
  QImage shade() const;
  void exportObj() const;

  SF2 slopeMap() const;
  SF2 drainage() const;
};

#endif
