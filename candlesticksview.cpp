#include "candlesticksview.h"

candleSticksView::candleSticksView(QChart *chart, QWidget *parent) :
    QChartView(chart, parent)
    , _ready(false)
{
    setRubberBand(QChartView::RectangleRubberBand);  // drag a rectangle to zoom in
    setCursor(Qt::CrossCursor);
}

void candleSticksView::setReady(bool ready)
{
    _ready = ready;
}

// Tooltip with the time and the price under the mouse
void candleSticksView::mouseMoveEvent(QMouseEvent *event)
{
    const QList<QAbstractAxis *> axes = chart()->axes(Qt::Horizontal);
    if (_ready && !axes.isEmpty()) {
        const QPointF point = chart()->mapToValue(event->position());
        auto *axisX = qobject_cast<QBarCategoryAxis *>(axes.first());
        const int index = qRound(point.x());
        const QString time = (axisX && index >= 0 && index < axisX->categories().size())
                                 ? axisX->categories().at(index)
                                 : QString();
        QToolTip::showText(event->globalPosition().toPoint(),
                           time + "\n" + QString::number(point.y(), 'f', point.y() >= 1 ? 2 : 8),
                           this, rect());
    }
    QChartView::mouseMoveEvent(event);
}

// Mouse wheel zooms in and out
void candleSticksView::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0)
        chart()->zoomIn();
    else
        chart()->zoomOut();

    event->accept();
}
