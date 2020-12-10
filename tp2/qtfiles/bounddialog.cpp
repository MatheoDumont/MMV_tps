#include "bounddialog.h"
#include "ui_bounddialog.h"

#include <QList>

BoundDialog::BoundDialog(QWidget *parent) : QDialog(parent), ui(new Ui::BoundDialog)
{
    ui->setupUi(this);
}

BoundDialog::~BoundDialog()
{
    delete ui;
}

void BoundDialog::getDoubles(double &min, double &max, double &boxsize) const
{
    min = ui->boundmin_val->value();
    max = ui->boundmax_val->value();
    boxsize = ui->boxsize_val->value();
}

QList<double> BoundDialog::getDoubles() const
{
    QList<double> doubles;

    doubles.append(ui->boundmin_val->value());
    doubles.append(ui->boundmax_val->value());
    doubles.append(ui->boxsize_val->value());

    return doubles;
}
