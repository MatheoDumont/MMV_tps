#include <iostream>
#include <QImage>

#include "vec.h"
#include "box2d.h"
#include "height_field.h"

int main(int argc, char const *argv[])
{
    QImage image("data/v_height_map.png");
    Box2D box(vec2(0.0, 0.0), 100.0);

    HeightField hf(image, box, 0.0, 100.0);

    return 0;
}
