#include "QChartMouseEvent.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QValueAxis>
#include <QPainter>

QChartMouseEvent::QChartMouseEvent(QWidget* parent) : QChartView(parent) {


}

QChartMouseEvent::~QChartMouseEvent() {}

void QChartMouseEvent::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPress = true;
        m_lastPoint = event->pos();
//        this->setRubberBand(QChartView::RectangleRubberBand);
        QPointF curVal = this->chart()->mapToValue(event->pos());
        emit mousePress(curVal);

//		if (!m_coordPointFront) {
//			m_coordPointFront = new QGraphicsSimpleTextItem(this->chart());
//			m_coordPointFront->setZValue(5);
//			m_coordPointFront->show();
//		}
//		m_coordPointFront->setVisible(true);

//        /// 获取纵轴
//		QValueAxis* axisY = (QValueAxis*)chart()->axisY();
//		QPointF curTemp = this->chart()->mapToPosition(QPointF(curVal.x(), axisY->max()));
//		m_coordPointFront->setPos(curTemp.x(), curTemp.y());
//		m_coordPointFront->setText(QString("X:%1").arg(curVal.x()));
    }
    QChartView::mousePressEvent(event);
}
void QChartMouseEvent::saveAxisRange()
{
    QValueAxis *axisX = dynamic_cast<QValueAxis*>(this->chart()->axisX());
    m_xMin = axisX->min();
    m_xMax = axisX->max();
    QValueAxis *axisY = dynamic_cast<QValueAxis*>(this->chart()->axisY());
    m_yMin = axisY->min();
    m_yMax = axisY->max();
}
void QChartMouseEvent::mouseMoveEvent(QMouseEvent* event)
{
    const QPoint curpos = event->pos();

    if (!m_coordItem) {
        m_coordItem = new QGraphicsSimpleTextItem(this->chart());
        m_coordItem->setZValue(5);
        m_coordItem->show();
    }
    m_coordItem->setVisible(true);
    const QPoint curPos = event->pos();
    QPointF curVal = this->chart()->mapToValue(curPos);
    QString coordStr = QString("X = %1\nY = %2").arg(curVal.x()).arg(curVal.y());
    m_coordItem->setText(coordStr);
//    m_coordItem->setFont(const QFont &font);
    m_coordItem->setPos(curPos.x(), curPos.y() - 40);

    emit mouseMoving(curVal);


    if (m_isPress) {
        QPoint offset = curpos - m_lastPoint;
        m_lastPoint = curpos;
        if (!m_alreadySaveRange)
        {
            this->saveAxisRange();
            m_alreadySaveRange = true;
        }
        this->chart()->scroll(-offset.x(), offset.y());
    }

    QChartView::mouseMoveEvent(event);
}
void QChartMouseEvent::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_isPress) {
        m_isPress = !m_isPress;
        QPointF curVal = this->chart()->mapToValue(event->pos());
        emit mouseLeftRelease(curVal);

//		if (!m_coordPointBack) {
//			m_coordPointBack = new QGraphicsSimpleTextItem(this->chart());
//			m_coordPointBack->setZValue(5);
//			m_coordPointBack->show();
//		}
//		m_coordPointBack->setVisible(true);

//        /// 获取纵轴
//		QValueAxis* axisY = (QValueAxis*)chart()->axisY();
//		QPointF curTemp = this->chart()->mapToPosition(QPointF(curVal.x(), axisY->max()));
//		m_coordPointBack->setPos(curTemp.x(), curTemp.y());
//		m_coordPointBack->setText(QString("X:%1").arg(curVal.x()));
    }
    if (event->button() == Qt::RightButton) {
        emit mouseRightRelease();
//		m_coordPointBack->setVisible(false);
//		m_coordPointFront->setVisible(false);

        if (m_alreadySaveRange)
        {
            this->chart()->axisX()->setRange(m_xMin, m_xMax);
            this->chart()->axisY()->setRange(m_yMin, m_yMax);
        }

    }
//    this->setRubberBand(QChartView::NoRubberBand);
    QChartView::mouseReleaseEvent(event);
}
void QChartMouseEvent::leaveEvent(QEvent*)
{
    emit mouseLeave();
    m_coordItem->setVisible(false);
}
void QChartMouseEvent::wheelEvent(QWheelEvent *pEvent)
{

    if (!m_alreadySaveRange)
    {
        this->saveAxisRange();
        m_alreadySaveRange = true;
    }


    qreal rVal = std::pow(0.999, pEvent->delta()); // 设置比例
    // 1. 读取视图基本信息
    QRectF oPlotAreaRect = this->chart()->plotArea();
    QPointF oCenterPoint = oPlotAreaRect.center();
    // 2. 水平调整
    oPlotAreaRect.setWidth(oPlotAreaRect.width() * rVal);
    // 3. 竖直调整
    oPlotAreaRect.setHeight(oPlotAreaRect.height() * rVal);
    // 4.1 计算视点，视点不变，围绕中心缩放
    //QPointF oNewCenterPoint(oCenterPoint);
    // 4.2 计算视点，让鼠标点击的位置移动到窗口中心
    //QPointF oNewCenterPoint(pEvent->pos());
    // 4.3 计算视点，让鼠标点击的位置尽量保持不动(等比换算，存在一点误差)
    QPointF oNewCenterPoint(2 * oCenterPoint - pEvent->pos() - (oCenterPoint - pEvent->pos()) / rVal);
    // 5. 设置视点
    oPlotAreaRect.moveCenter(oNewCenterPoint);
    // 6. 提交缩放调整
    this->chart()->zoomIn(oPlotAreaRect);
    QChartView::wheelEvent(pEvent);
}
//设置区域
void QChartMouseEvent::setRectAera(QRectF rc, int width)
{
    if(m_Rectframe != nullptr){
        return;
    }
    m_Rectframe = new QGraphicsRectItem(this->chart());
    // 设置画笔、画刷
    QPen pen = m_Rectframe->pen();
    pen.setWidth(width);
    pen.setColor(Qt::red);
    pen.setStyle (Qt::DashDotDotLine);
    m_Rectframe->setPen(pen);

    // 矩形区域 起点：(50, 50) 宽：100 高：100 100,100,1000,200
    m_Rectframe->setRect(rc);
    m_Rectframe->show();
}
