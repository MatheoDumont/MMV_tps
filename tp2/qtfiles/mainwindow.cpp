#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), bd(this), type(TypeOfDisplay::Grayscale)
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

    if (min >= max && boxsize <= 1)
        return;

    ui->statusbar->showMessage("Loading: \"" + filename + "\"", 1500);
    image = QImage(filename);
    Box2D box(vec2(0.0), vec2(boxsize));
    hf_base = HeightField(image, box, min, max);

    displayImage(hf_base);

    ui->actionSave_image->setEnabled(true);
    ui->action3D_model->setEnabled(true);

    // Création du mesh
    ui->openGL_viewer->vertices.clear();
    ui->openGL_viewer->colors.clear();
    ui->openGL_viewer->normals.clear();

    hf_base.getMesh(ui->openGL_viewer->vertices,
                    ui->openGL_viewer->colors,
                    ui->openGL_viewer->normals);

    ui->openGL_viewer->maxHeight = hf_base.maxHeight;
}

void MainWindow::displayGL(const HeightField &hf)
{
    ui->openGL_viewer->updateMeshColor(hf);
    ui->openGL_viewer->paintGL();
}

void MainWindow::displayImage(const HeightField &hf)
{
    QPixmap res;

    switch (this->type)
    {
    case TypeOfDisplay::Grayscale:
        res = QPixmap::fromImage(hf.grayscale());
        break;
    case TypeOfDisplay::HSV:
        // TODO enlever parametres en dur et mettre un selecteur dans ui
        res = QPixmap::fromImage(hf.colorHSV(270, 330));
        break;
    case TypeOfDisplay::Coloring:
        res = QPixmap::fromImage(hf.color());
        break;
    default:
        break;
    }

    int w = ui->image_viewer->width();
    int h = ui->image_viewer->height();
    ui->image_viewer->setPixmap(res.scaled(w, h, Qt::KeepAspectRatio));
}

void MainWindow::display(const HeightField &hf)
{
    if (ui->openGL_viewer->getIsDisplayed())
        displayGL(hf);
    else
        displayImage(hf);
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

void MainWindow::on_StreamAreaD8_Button_clicked()
{
    SF sf = hf_base.drainage(HeightField::StreamAreaFunc::D8);
    hf_transforme = HeightField(sf);
    display(hf_transforme);
}

void MainWindow::on_StreamAreaSteepestButton_clicked()
{
    SF sf = hf_base.drainage(HeightField::StreamAreaFunc::Steepest);
    hf_transforme = HeightField(sf);
    display(hf_transforme);
}

void MainWindow::on_grayscaling_clicked()
{
    this->type = TypeOfDisplay::Grayscale;
    display(hf_base);
}

void MainWindow::on_standardColoring_clicked()
{
    this->type = TypeOfDisplay::Coloring;
    display(hf_base);
}

void MainWindow::on_HSVIng_clicked()
{
    this->type = TypeOfDisplay::HSV;
    display(hf_base);
}

void MainWindow::on_smoothing_clicked()
{
    SF sf = SF::filter(hf_base, SF::FilterType::Smooth);
    hf_base = HeightField(sf);

    if (ui->openGL_viewer->getIsDisplayed())
        hf_base.getMesh(ui->openGL_viewer->vertices,
                        ui->openGL_viewer->colors,
                        ui->openGL_viewer->normals);
    display(hf_base);
}

void MainWindow::on_bluring_clicked()
{
    SF sf = SF::filter(hf_base, SF::FilterType::Blur);
    hf_base = HeightField(sf);
    
    if (ui->openGL_viewer->getIsDisplayed())
        hf_base.getMesh(ui->openGL_viewer->vertices,
                        ui->openGL_viewer->colors,
                        ui->openGL_viewer->normals);
    display(hf_base);
}
