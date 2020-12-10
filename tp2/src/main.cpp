#include "mainwindow.h"
#include <QApplication>

/* /!\ Version de Qt à utiliser : 5.12 /!\ */

// Bounds de Vanoise : 297.0, 3825.0

/**
 * TODO:
 *     - Possibilité de charger une nouvelle image après en avoir chargé une
 *          - update du VBO (+ potentiellement le resize)
 *     - Lissage masque 3*3 et Blur
 *     - Fonction Clamp
 *     - export exportObj
 *          - *lecture de doc intensifies*
 *     - Shade dans HeightField
 *     - Rotation dans vue 3D
 *     - couleurs 
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;

    w.show();
    return app.exec();
}
