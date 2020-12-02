#include <iostream>
#include <QImage>

#include "vec.h"
#include "box2d.h"
#include "height_field.h"

// /!\ Version de Qt à utiliser : 5.12 /!\

/**
 * TODO:
 *     - Rendre le truc un peu plus paramétrable (genre dire en CLI l'image et les infos de bounds, ...)
 *     - Faire un affichage sous Qt de l'image (ou directement faire le rendu 3D)
 *          - QApplication avec une QWindow devrait être bon pour du OpenGL (modifier CMakeLists.txt -> suppromer Qt::OpenGL et mettre Qt::GUI à la place)
 *     - Lissage masque 3*3 et Blur
 *     - Fonction Clamp
 *     - export exportObj
 *          - *lecture de doc intensifies*
 *     - Shade dans HeightField
 */ // sugoi sugoi
int main(int argc, char const *argv[])
{
    QImage image("data/v_height_map.png");
    Box2D box(vec2(0.0, 0.0), vec2(image.width(), image.height()));

    HeightField hf(image, box, 297.0, 3825.0);

    QImage res = hf.grayscale();
    res.save("res.png");

    SF sf = hf.laplacianMap();
    HeightField(sf).grayscale().save("res_laplacian.png"); // grayscale pas ouf

    sf = hf.slopeMap();
    HeightField(sf).grayscale().save("res_slope.png");

    return 0;
}
