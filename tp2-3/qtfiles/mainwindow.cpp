#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), bd(this), type(HeightField::Grayscale), error_message(this)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentWidget(ui->image_display);

    connect(&bd, SIGNAL(accepted()), this, SLOT(on_boundsSpecified()));
    connect(ui->image_viewer, SIGNAL(imageClicked(int, int, Qt::MouseButton)), this, SLOT(imageClicked(int, int, Qt::MouseButton)));
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

    specificDisplay = Default;

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
    road_start = QPoint(-1, -1);
    road_end = QPoint(-1, -1);
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
    QImage im;

    switch (this->type)
    {
    case HeightField::ColorType::Grayscale:
        im = hf_color.grayscale();
        break;

    case HeightField::ColorType::HSV:
        // TODO enlever parametres en dur et mettre un selecteur dans ui
        im = hf_color.colorHSV(270, 330);
        break;

    case HeightField::ColorType::Coloring:
        im = hf_color.color(specificDisplay);
        break;

    default:
        break;
    }

    im = updateImage(im);
    res = QPixmap::fromImage(im);

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

void MainWindow::on_RoadAction_clicked()
{
    if (road_start.x() == -1 || road_start.y() == -1)
    {
        error_message.showMessage("The starting point of the road is not set");
        return;
    }
    else if (road_end.x() == -1 || road_end.y() == -1)
    {
        error_message.showMessage("The ending point of the road is not set");
        return;
    }
    else if (road_start == road_end)
    {
        error_message.showMessage("The starting point and the ending point are the same");
        return;
    }

    int k = 4;
    double slope_coef = ui->spinSlope->value();

    Road r = Road(hf_topology, k, slope_coef);

    on_action3D_model_triggered();
    display();

    std::list<vertex_t> path = r.compute({road_start.x(), road_start.y()}, {road_end.x(), road_end.y()});

    bool b = r.drawLine(ui->openGL_viewer->colors, path);
    if (b == false)
    {
        on_actionImage_view_triggered();
        display();
        error_message.showMessage("Can't find a path with those parameters");
        return;
    }

    ui->openGL_viewer->updateColorBuffer();
    ui->openGL_viewer->paintGL();
}

void MainWindow::imageClicked(int x, int y, Qt::MouseButton button)
{
    if (ui->image_viewer->pixmap()->width() == 0 ||
        ui->image_viewer->pixmap()->height() == 0)
        return;

    int x_im = x * image.width() / ui->image_viewer->pixmap()->width();
    int y_im = y * image.height() / ui->image_viewer->pixmap()->height();

    switch (button)
    {
    case Qt::MouseButton::LeftButton:
        road_start = QPoint(x_im, y_im);
        break;

    case Qt::MouseButton::RightButton:
        road_end = QPoint(x_im, y_im);
        break;

    default:
        break;
    };

    display();
}

QImage MainWindow::updateImage(const QImage &im)
{
    QImage temp_color = im.copy(0, 0, im.width(), im.height());
    temp_color = temp_color.convertToFormat(QImage::Format_RGB32);

    if (road_start.x() != -1 && road_start.y() != -1)
        temp_color.setPixelColor(road_start, QColor(0, 255, 0));

    if (road_end.x() != -1 && road_end.y() != -1)
        temp_color.setPixelColor(road_end, QColor(255, 0, 0));

    return temp_color;
}
