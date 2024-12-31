#ifndef RADIALPOLARCHART_H
#define RADIALPOLARCHART_H

#include <QWidget>
#include <QtCharts>
#include <QPolarChart>
#include <QChartView>
#include <QSplineSeries>
#include <myhelper.h>
#include <titlewidget.h>

#include "chartview.h"

namespace Ui {
class radialPolarChart;
}

class radialPolarChart : public QWidget
{
    Q_OBJECT

public:
    explicit radialPolarChart(QWidget *parent = nullptr);
    ~radialPolarChart();
    void initPolarChart(QWidget *widget, QPolarChart *polarChart, QChartView *view, QSplineSeries *series, QScatterSeries *scatterSeries);
    void setRectAera(QRectF rc, int width);
    QWidget *widget;
    bool isCreated;
    QSplineSeries *polarSeries;
    QScatterSeries *scatter;
//    QChartView *polarChartView;

    ChartView *polarChartView;
    QPolarChart *chart;
    QPolarChart *polarChart;

    QList<QPointF> FlagPosList;
    void drawAnnotations(const QRectF &rc);
private:
    Ui::radialPolarChart *ui;
    QGraphicsRectItem* m_Rectframe = nullptr;



};

#endif // RADIALPOLARCHART_H
