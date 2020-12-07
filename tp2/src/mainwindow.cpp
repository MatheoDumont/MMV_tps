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
    double min, max;
    bd.getDoubles(min, max);

    if (min > max)
        return;

    ui->statusbar->showMessage("Loading: \"" + filename + "\"", 1000);
    image = QImage(filename);
    Box2D box(vec2(0.0, 0.0), vec2(image.width(), image.height()));
    hf = HeightField(image, box, min, max);

    QPixmap res = QPixmap::fromImage(hf.grayscale());
    int w = ui->image_viewer->width();
    int h = ui->image_viewer->height();
    ui->image_viewer->setPixmap(res.scaled(w, h, Qt::KeepAspectRatio));

    ui->actionSave_image->setEnabled(true);
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
    QApplication::quit();
}

void MainWindow::on_actionImage_view_triggered()
{
    ui->stackedWidget->setCurrentWidget(ui->image_display);
}

void MainWindow::on_action3D_model_triggered()
{
    ui->stackedWidget->setCurrentWidget(ui->opengl_display);
}

void MainWindow::on_action_StreamAreaD8_triggered()
{
    SF sf = hf.drainage(0);
    std::cout << "dedans "<< std::endl;

    HeightField drain(sf);
    QPixmap res = QPixmap::fromImage(drain.grayscale());
    int w = ui->image_viewer->width();
    int h = ui->image_viewer->height();
    ui->image_viewer->setPixmap(res.scaled(w, h, Qt::KeepAspectRatio));

    ui->actionSave_image->setEnabled(true);
}
