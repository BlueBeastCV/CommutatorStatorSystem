#ifndef MYDASHBOARD_H
#define MYDASHBOARD_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
class myDashBoard : public QWidget
{
    Q_OBJECT
public:
    explicit myDashBoard(QWidget *parent = nullptr);

    void setValue(double val);
    QColor m_background;
    QColor m_foreground;

    int m_maxValue;
    int m_minValue;
    int m_startAngle;
    int m_endAngle;

    int m_scaleMajor;
    int m_scaleMinor;
    double m_value;
    int m_precision;
    QTimer *m_updateTimer;
    QString m_units;
    QString m_title;
signals:

public slots:
    void UpdateAngle();
protected:
    void paintEvent(QPaintEvent *);

    void drawCrown(QPainter *painter);
    void drawBackground(QPainter *painter);
    void drawScale(QPainter *painter);
    void drawScaleNum(QPainter *painter);
    void drawTitle(QPainter *painter);
    void drawIndicator(QPainter *painter);
    void drawNumericValue(QPainter *painter);

private:

};

#endif // MYDASHBOARD_H
