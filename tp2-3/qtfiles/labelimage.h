#ifndef LABELIMAGE_H
#define LABELIMAGE_H

#include <QLabel>
#include <QMouseEvent>

class LabelImage : public QLabel
{
    Q_OBJECT

public:
    explicit LabelImage(QWidget *parent = nullptr);

public:
    void mousePressEvent(QMouseEvent *ev);

signals:
    void imageClicked(int x, int y, Qt::MouseButton button);
};

#endif // LABELIMAGE_H
