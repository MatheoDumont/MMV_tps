#include "mainwindow.h"
#include <QApplication>

/* /!\ Version de Qt à utiliser : 5.12 /!\ */

// Bounds de Vanoise : 297.0, 3825.0

/**
 * TODO:
 *     - Faire un affichage sous Qt de l'image (ou directement faire le rendu 3D)
 *          - QApplication avec une QWindow devrait être bon pour du OpenGL
 *     - Lissage masque 3*3 et Blur
 *     - Fonction Clamp
 *     - export exportObj
 *          - *lecture de doc intensifies*
 *     - Shade dans HeightField
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;

    w.show();
    return app.exec();

}
