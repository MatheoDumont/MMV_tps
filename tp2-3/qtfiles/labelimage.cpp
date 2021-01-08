#include "labelimage.h"

#include <QPoint>
#include <iostream>

LabelImage::LabelImage(QWidget *parent) : QLabel(parent)
{
}

void LabelImage::mousePressEvent(QMouseEvent *ev)
{
    const QPoint& pos = mapFrom(this, ev->pos());
    const Qt::MouseButton button = ev->button();

    if (button == Qt::MouseButton::LeftButton || button == Qt::MouseButton::RightButton)
        emit imageClicked(pos.x(), pos.y(), button);
}
