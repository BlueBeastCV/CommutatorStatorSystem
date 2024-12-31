#ifndef QCHARTMOUSEEVENT_H
#define QCHARTMOUSEEVENT_H
#include <QChartView>
#include <QMouseEvent>
#include <QGraphicsSimpleTextItem>
#include <QGestureEvent>

QT_CHARTS_USE_NAMESPACE

class QChartMouseEvent : public QChartView
{
    Q_OBJECT

public:
    QChartMouseEvent(QWidget* parent = nullptr);
    ~QChartMouseEvent();

    void saveAxisRange();
    void setRectAera(QRectF rc, int width);
signals:
    void mouseMoving(QPointF point);
    void mousePress(QPointF point);
    void mouseLeftRelease(QPointF point);
    void mouseRightRelease();
    void mouseLeave();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void wheelEvent(QWheelEvent *pEvent) override;
private:
    bool m_isPress = false;
    bool m_alreadySaveRange = false;
    double m_xMin, m_xMax, m_yMin, m_yMax = 0.0;
    QPoint m_lastPoint;
    QGraphicsSimpleTextItem* m_coordItem = nullptr;
    QGraphicsSimpleTextItem* m_coordPointFront = nullptr;
    QGraphicsSimpleTextItem* m_coordPointBack = nullptr;

    QGraphicsRectItem* m_Rectframe = nullptr;
};

#endif // QCHARTMOUSEEVENT_H
