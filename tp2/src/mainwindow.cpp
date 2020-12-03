#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QFileDialog>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_image_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open an height image", QDir::currentPath());

    if (!filename.isEmpty())
      std::cout << "Filename : " << filename.toStdString() << std::endl;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::quit();
}
