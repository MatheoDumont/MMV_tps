#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), bd(this)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentWidget(ui->image_display);

    connect(&bd, SIGNAL(accepted()), this, SLOT(on_boundsSpecified()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_boundsSpecified()
{
    double min, max, boxsize;
    bd.getDoubles(min, max, boxsize);

    if (min > max)
        return;

    ui->statusbar->showMessage("Loading: \"" + filename + "\"", 1500);
    image = QImage(filename);
    Box2D box(vec2(0.0), vec2(boxsize));
    hf_base = HeightField(image, box, min, max);

    QPixmap res = QPixmap::fromImage(hf_base.grayscale());
    int w = ui->image_viewer->width();
    int h = ui->image_viewer->height();
    ui->image_viewer->setPixmap(res.scaled(w, h, Qt::KeepAspectRatio));

    ui->actionSave_image->setEnabled(true);
    ui->action3D_model->setEnabled(true);

    // Création du mesh
    hf_base.getMesh(ui->openGL_viewer->vertices,
                    ui->openGL_viewer->colors,
                    ui->openGL_viewer->normals);

    ui->openGL_viewer->maxHeight = hf_base.maxHeight;
}

void MainWindow::on_actionOpen_image_triggered()
{
    filename = QFileDialog::getOpenFileName(this, "Open an height image", QDir::currentPath(), filter);

    if (!filename.isEmpty())
        bd.exec();
}

void MainWindow::on_actionSave_image_triggered()
{
    if (image.isNull() != true)
    {
        QString outfile = QFileDialog::getSaveFileName(this, "Save the height image", QDir::currentPath());
        if (!outfile.isEmpty())
            image.save(outfile);
    }
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::on_actionImage_view_triggered()
{
    ui->stackedWidget->setCurrentWidget(ui->image_display);
    ui->openGL_viewer->setIsDisplayed(false);
}

void MainWindow::on_action3D_model_triggered()
{
    ui->stackedWidget->setCurrentWidget(ui->opengl_display);
    ui->openGL_viewer->setIsDisplayed(true);
}

void MainWindow::on_StreamArea(int func)
{
    SF sf = hf_base.drainage(func);
    hf_transforme(sf);

    if (ui->openGL_viewer->getIsDisplayed())
    {
        ui->openGL_viewer->updateMeshColor(hf_transforme);
        ui->openGL_viewer->paintGL();
    }
    else
    {
        QPixmap res = QPixmap::fromImage(hf_transforme.grayscale());
        int w = ui->image_viewer->width();
        int h = ui->image_viewer->height();
        ui->image_viewer->setPixmap(res.scaled(w, h, Qt::KeepAspectRatio));
    }
}
void MainWindow::on_StreamAreaD8_Button_clicked()
{
    on_StreamArea(0);
}

void MainWindow::on_StreamAreaSteepestButton_clicked()
{
    on_StreamArea(1);
}
