#ifndef MYCHARTVIEW_H
#define MYCHARTVIEW_H
#include <QtCharts>

class myChartView : public QChartView
{
    Q_OBJECT
public:
    myChartView(QWidget *parent = Q_NULLPTR);
    ~myChartView();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
signals:
    void signalMouseEvent(int eventId, QMouseEvent *event);
    void signalWheelEvent(QWheelEvent *event);
};

#endif // MYCHARTVIEW_H
