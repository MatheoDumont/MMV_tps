#ifndef BOUNDDIALOG_H
#define BOUNDDIALOG_H

#include <QDialog>

namespace Ui {
    class BoundDialog;
}

class BoundDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BoundDialog(QWidget *parent = nullptr);
    ~BoundDialog();

    void getDoubles(double &min, double &max) const;
    QList<double> getDoubles() const;
    
private:
    Ui::BoundDialog *ui;
};

#endif // BOUNDDIALOG_H
