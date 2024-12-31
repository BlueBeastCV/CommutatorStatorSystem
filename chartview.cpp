/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Charts module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "chartview.h"
#include <QtGui/QMouseEvent>
#include <QtCore/QDebug>
#include <QtCharts/QAbstractAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QSplineSeries>


QT_CHARTS_USE_NAMESPACE

ChartView::ChartView(QWidget *parent)
    : QChartView(parent)
{

}

//![1]
void ChartView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        chart()->zoomIn();
        break;
    case Qt::Key_Minus:
        chart()->zoomOut();
        break;
    case Qt::Key_Left:
        chart()->scroll(-1.0, 0);
        break;
    case Qt::Key_Right:
        chart()->scroll(1.0, 0);
        break;
    case Qt::Key_Up:
        chart()->scroll(0, 1.0);
        break;
    case Qt::Key_Down:
        chart()->scroll(0, -1.0);
        break;
    case Qt::Key_Space:
        break;
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}

void ChartView::resizeEvent(QResizeEvent* event)
{
    QChartView::resizeEvent(event); // 调用基类的resizeEvent
    // 在这里添加你的处理代码
    for(int i = 0; i < PixmapItemList.size(); i++){
        this->scene()->removeItem(PixmapItemList.at(i));
    }
    showLabels();
}

uint qHash(const QPointF &key, uint seed)
{
    return qHash(qMakePair(key.x(), key.y()), seed);
}
//![1]
void ChartView::setFlagPosList(const QHash<QPointF, PointType> &flagPosList)
{
    FlagPosList = flagPosList;
}

void ChartView::showLabels()
{
    auto series = static_cast<QSplineSeries*>(this->chart()->series().at(0));
    QList<QPointF> points = series->points();
    QList<QPointF> posList;

    for (int i = 0; i < points.size(); ++i) {
        QPointF point = chart()->mapToPosition(points.at(i));
        posList.append(point);

        if (FlagPosList.contains(points.at(i))) {
            PointType type = FlagPosList[points.at(i)];
            QString iconPath;
            switch (type) {
            case HighPoint:
                iconPath = "triangle.png";
                break;
            case LowPoint:
                iconPath = "square.png";
                break;
            case Tilt:
                iconPath = "plus.png";
                break;
            case Eccentric:
                iconPath = "star.png";
                break;
            case DataDistribution:
                iconPath = "circle.png";
                break;
            default:
                break;
            }
            QGraphicsPixmapItem *textItem = new QGraphicsPixmapItem();
            PixmapItemList.append(textItem);
            textItem->setPixmap(QPixmap(iconPath));
            textItem->setPos(posList.at(i).x() - 9, posList.at(i).y() - 9);
            this->scene()->addItem(textItem);
//            qDebug() << "Adding label at" << point << "with type" << type;
        }
    }

}

//设置区域
void ChartView::setRectAera(QRectF rc, int width)
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
//![2]
