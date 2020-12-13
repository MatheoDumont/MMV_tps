#include "mainwindow.h"
#include <QApplication>

/**
 * TODO:
 *     - Fonction Clamp (vec3)
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
