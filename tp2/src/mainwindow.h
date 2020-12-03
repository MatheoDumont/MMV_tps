#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QString>

#include "bounddialog.h"
#include "height_field.h"

namespace Ui {
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

private:
    Ui::MainWindow *ui;

    BoundDialog bd;
    QString filename;
    QString filter = "Image (*.jpe *.jpg *.jpeg *.png *.tif *.tiff *.raw)";
    QImage image;
    
    HeightField hf;
};

#endif // MAINWINDOW_H
