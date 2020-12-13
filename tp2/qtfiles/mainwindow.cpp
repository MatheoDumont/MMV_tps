#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), bd(this), type(HeightField::Grayscale)
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

    if (min >= max || boxsize <= 1)
        return;

    ui->statusbar->showMessage("Loading: \"" + filename + "\"", 1500);
    image = QImage(filename);
    Box2D box(vec2(0.0), vec2(boxsize));

    hf_topology = HeightField(image, box, min, max);
    hf_color = HeightField(hf_topology);

    displayImage();

    ui->actionSave_image->setEnabled(true);
    ui->action3D_model->setEnabled(true);

    // Création du mesh
    ui->openGL_viewer->vertices.clear();
    ui->openGL_viewer->colors.clear();
    ui->openGL_viewer->normals.clear();

    hf_topology.getMesh(ui->openGL_viewer->maxHeight,
                        ui->openGL_viewer->vertices,
                        ui->openGL_viewer->colors,
                        ui->openGL_viewer->normals);

    ui->openGL_viewer->maxHeight = hf_topology.maxHeight;
}

void MainWindow::displayGL()
{
    ui->openGL_viewer->updateMeshTopology(hf_topology);

    /**
     * TODO: enlever paramètres en dur et mettre un sélecteur dans l'UI
     */
    ui->openGL_viewer->updateMeshColor(hf_color, type, specificDisplay, 270, 330);

    ui->openGL_viewer->paintGL();
}

void MainWindow::displayImage()
{
    QPixmap res;

    switch (this->type)
    {
    case HeightField::ColorType::Grayscale:
        res = QPixmap::fromImage(hf_color.grayscale());
        break;

    case HeightField::ColorType::HSV:
        // TODO enlever parametres en dur et mettre un selecteur dans ui
        res = QPixmap::fromImage(hf_color.colorHSV(270, 330));
        break;

    case HeightField::ColorType::Coloring:
        res = QPixmap::fromImage(hf_color.color(specificDisplay));
        break;

    default:
        break;
    }

    int w = ui->image_viewer->width();
    int h = ui->image_viewer->height();
    ui->image_viewer->setPixmap(res.scaled(w, h, Qt::KeepAspectRatio));
}

void MainWindow::display()
{
    if (ui->openGL_viewer->getIsDisplayed())
        displayGL();
    else
        displayImage();
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
    SF sf = hf_topology.streamArea(HeightField::StreamAreaFunc::D8);
    hf_color = HeightField(sf);
    specificDisplay = StreamArea;
    display();
}

void MainWindow::on_StreamAreaSteepestButton_clicked()
{
    SF sf = hf_topology.streamArea(HeightField::StreamAreaFunc::Steepest);
    hf_color = HeightField(sf);
    specificDisplay = StreamArea;
    display();
}

void MainWindow::on_StreamPowerButton_clicked()
{
    SF sf = hf_topology.streamPower();
    hf_color = HeightField(sf);
    specificDisplay = StreamPower;
    display();
}

void MainWindow::on_WetnessIndexButton_clicked()
{
    SF sf = hf_topology.wetnessIndex();
    hf_color = HeightField(sf);
    specificDisplay = WetnessIndex;
    display();
}

void MainWindow::on_grayscaling_clicked()
{
    this->type = HeightField::ColorType::Grayscale;
    display();
}

void MainWindow::on_standardColoring_clicked()
{
    this->type = HeightField::ColorType::Coloring;
    display();
}

void MainWindow::on_HSVIng_clicked()
{
    this->type = HeightField::ColorType::HSV;
    display();
}

void MainWindow::on_smoothing_clicked()
{
    SF sf = SF::filter(hf_topology, SF::FilterType::Smooth);
    hf_topology = HeightField(sf);
    hf_color = HeightField(hf_topology);
    display();
}

void MainWindow::on_bluring_clicked()
{
    SF sf = SF::filter(hf_topology, SF::FilterType::Blur);
    hf_topology = HeightField(sf);
    hf_color = HeightField(hf_topology);
    display();
}
