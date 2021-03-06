#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QErrorMessage>
#include <QImage>
#include <QPoint>
#include <QString>

#include "bounddialog.h"
#include "height_field.h"
#include "road.h"
#include "labelimage.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_boundsSpecified();

    // File menu
    void on_actionOpen_image_triggered();
    void on_actionSave_image_triggered();
    void on_actionQuit_triggered();

    // View menu
    void on_actionImage_view_triggered();
    void on_action3D_model_triggered();

    void on_StreamAreaD8_Button_clicked();
    void on_StreamAreaSteepestButton_clicked();

    void on_grayscaling_clicked();
    void on_standardColoring_clicked();
    void on_HSVIng_clicked();

    void on_smoothing_clicked();
    void on_bluring_clicked();

    /*
     * display() appelle en fonction de ui->openGL_viewer->getIsDisplayed():
     *      - displayImage
     *      - displayGL
     */
    void display();
    void displayImage();
    void displayGL();

    void on_StreamPowerButton_clicked();

    void on_WetnessIndexButton_clicked();

    void on_RoadAction_clicked();

    void imageClicked(int x, int y, Qt::MouseButton button);
    QImage updateImage(const QImage& im);

private:
    Ui::MainWindow *ui;

    BoundDialog bd;
    QString filename;
    QString filter = "Image (*.jpe *.jpg *.jpeg *.png *.tif *.tiff *.raw)";
    QImage image;
    QPoint road_start = QPoint(-1, -1);
    QPoint road_end = QPoint(-1, -1);
    QErrorMessage error_message;

    /*
     * On sauvegarde dedans les informations de la hauteur
     * C'est lui qu'on modifie avec les filtres smooth et blur
     * On l'utilise aussi pour construire le maillage et les normals.
     */
    HeightField hf_topology;

    /*
     * On sauvegarde dedans uniquement les informations qui vont nous servir
     * a calculer les images / textures.
     */
    HeightField hf_color;

    HeightField::ColorType type;

    SpecificDisplay specificDisplay = Default;

    friend class DisplayGLWidget;
};

#endif // MAINWINDOW_H
