#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QString>

#include "bounddialog.h"
#include "height_field.h"

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
    void display(const HeightField &hf);
    void displayImage(const HeightField &hf);
    void displayGL(const HeightField &hf);

private:
    Ui::MainWindow *ui;

    BoundDialog bd;
    QString filename;
    QString filter = "Image (*.jpe *.jpg *.jpeg *.png *.tif *.tiff *.raw)";
    QImage image;

    HeightField hf_base;
    HeightField hf_transforme;

    enum TypeOfDisplay
    {
        Grayscale,
        HSV,
        Coloring
    };
    TypeOfDisplay type;
};

#endif // MAINWINDOW_H
