#include <iostream>
#include <QImage>

#include "vec.h"
#include "box2d.h"
#include "height_field.h"

int main(int argc, char const *argv[])
{
    QImage image("data/v_height_map.png");
    
    // Je sais pas quoi mettre pour la box donc
    // j'ai essayé de faire une box de la taille de l'image
    Box2D box(vec2(0.0, 0.0), vec2(image.width(), image.height()));

    // Je sais pas quoi mettre en boundmin et en boundmax
    HeightField hf(image, box, 0.0, 10.0);

    QImage res = hf.grayscale();
    res.save("res.png");

    return 0;
}
