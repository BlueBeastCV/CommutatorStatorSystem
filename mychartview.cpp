#include "mychartview.h"

myChartView::myChartView(QWidget *parent)
{

}

myChartView::~myChartView()
{

}

void myChartView::mousePressEvent(QMouseEvent *event) {
    emit signalMouseEvent(0, event);
    QChartView::mousePressEvent(event);
}

void myChartView::mouseMoveEvent(QMouseEvent *event) {
    emit signalMouseEvent(1, event);
    QChartView::mouseMoveEvent(event);
}

void myChartView::mouseReleaseEvent(QMouseEvent *event) {
    emit signalMouseEvent(2, event);
    QChartView::mouseReleaseEvent(event);
}

void myChartView::mouseDoubleClickEvent(QMouseEvent *event) {
    emit signalMouseEvent(3, event);
    QChartView::mouseDoubleClickEvent(event);
}

void myChartView::wheelEvent(QWheelEvent *event) {
    emit signalWheelEvent(event);
    QChartView::wheelEvent(event);
}
