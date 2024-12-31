#ifndef AXIALPOLARCHART_H
#define AXIALPOLARCHART_H

#include <QWidget>
#include <QtCharts>
#include <QPolarChart>
#include <QChartView>
#include <QSplineSeries>
#include <myhelper.h>
#include <titlewidget.h>
#include "chartview.h"
namespace Ui {
class axialpolarChart;
}

class axialpolarChart : public QWidget
{
    Q_OBJECT

public:
    explicit axialpolarChart(QWidget *parent = nullptr);
    ~axialpolarChart();
    void initPolarChart(QWidget *widget, QPolarChart *polarChart, QChartView *view, QSplineSeries *series, QScatterSeries *scatterSeries);
    void setRectAera(QRectF rc, int width);
    QWidget *widget;
    bool isCreated;
    QSplineSeries *polarSeries;
    QScatterSeries *scatter;
//    QChartView *polarChartView;
    ChartView *polarChartView;
    QPolarChart *polarChart;
    void drawAnnotations(const QRectF &rc);

private:
    Ui::axialpolarChart *ui;
    QGraphicsRectItem* m_Rectframe = nullptr;
};

#endif // AXIALPOLARCHART_H
