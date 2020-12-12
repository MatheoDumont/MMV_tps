#include "mainwindow.h"
#include <QApplication>

/* /!\ Version de Qt à utiliser : 5.12 /!\ */

// Bounds de Vanoise : 18000x18000, 297.0, 3825.0
// Bounds de la heightmap de Lucas : 1000x1000, 0.0, 500.0 

/**
 * TODO:
 *     - Lissage masque 3*3 et Blur
 *     - Fonction Clamp
 *     - export exportObj
 *          - *lecture de doc intensifies*
 *     - Shade dans HeightField
 *     - couleurs 
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;

    w.show();
    return app.exec();
}
