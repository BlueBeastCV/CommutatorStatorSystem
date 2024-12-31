#include "radialpolarchart.h"
#include "ui_radialpolarchart.h"

radialPolarChart::radialPolarChart(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::radialPolarChart)
{
    ui->setupUi(this);
    myHelper::FormInCenter(this);//窗体居中显示
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    ui->polarTitle->set_lab_Title("径向极坐标图");
    ui->polarTitle->setShowLabIco(false);
    ui->polarTitle->setShowMinBtn(true);
    ui->polarTitle->setShowMaxBtn(true);
    //    setFixedSize(this->width(),this->height());
    polarChart = new QPolarChart();
    polarChartView = new ChartView();
    polarSeries = new QSplineSeries();
    scatter = new QScatterSeries();
    initPolarChart(ui->polarWidget,polarChart,polarChartView,polarSeries,scatter);
    widget = ui->polarWidget;
    this->setRectAera(QRectF(50,50,150,180), 1);

//    iinitPolarChart();
}

radialPolarChart::~radialPolarChart()
{
    delete ui;
    isCreated = false;
}

void radialPolarChart::initPolarChart(QWidget *widget, QPolarChart *polarChart, QChartView *view, QSplineSeries *series, QScatterSeries *scatterSeries)
{
    polarChart->addSeries(series);
    polarChart->addSeries(scatterSeries);
    polarChart->legend()->hide();

    // 设置 QSplineSeries 属性
    QPen pen = series->pen();
    pen.setWidth(2);
    series->setPen(pen);
    series->setPointsVisible(true);

    // 设置 QScatterSeries 属性
    QPen scatterPen = scatterSeries->pen();
    scatterPen.setWidth(3);
    scatterSeries->setPen(scatterPen);
    scatterSeries->setPointsVisible(true);
    scatterSeries->setPointLabelsVisible(true);
    scatterSeries->setPointLabelsFormat("@yPointmm∠@xPoint°");

    // 创建角度轴
    QValueAxis *angularAxis = new QValueAxis();
    angularAxis->setTickCount(13);
    angularAxis->setLabelFormat("%d");
    angularAxis->setShadesVisible(false);
    angularAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
    polarChart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);

    // 设置角度轴的线笔和字体
    QPen angularPen(QColor(0, 0, 0)); // 黑色线条
    angularPen.setWidth(2); // 线条宽度
    angularAxis->setLinePen(angularPen); // 设置线条笔
    QFont angularFont;
    angularFont.setPointSize(8); // 字体大小
    angularFont.setBold(true); // 字体加粗
    angularAxis->setLabelsFont(angularFont); // 设置标签字体
    angularAxis->setLabelsColor(QColor(0, 0, 0)); // 设置标签颜色

    // 创建径向轴
    QValueAxis *radialAxis = new QValueAxis();
    radialAxis->setTickCount(7);
    radialAxis->setLabelFormat("%d");
    radialAxis->setRange(-1, 1);
    angularAxis->setRange(0, 360);

    // 设置径向轴的线笔和字体
    QPen radialPen(QColor(0, 0, 0)); // 黑色线条
    radialPen.setWidth(2); // 线条宽度
    radialAxis->setLinePen(radialPen); // 设置线条笔
    QFont radialFont;
    radialFont.setPointSize(8); // 字体大小
    radialFont.setBold(true); // 字体加粗
    radialAxis->setLabelsFont(radialFont); // 设置标签字体
    radialAxis->setLabelsColor(QColor(0, 0, 0)); // 设置标签颜色

    // 将轴绑定到图表
    polarChart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);
    series->attachAxis(radialAxis);
    series->attachAxis(angularAxis);
    scatterSeries->attachAxis(radialAxis);
    scatterSeries->attachAxis(angularAxis);

    polarChartView->setChart(polarChart);
    polarChartView->setRenderHint(QPainter::Antialiasing);

    // 设置渲染提示
    view->setRenderHint(QPainter::HighQualityAntialiasing);

    // 设置布局
    polarChart->setGeometry(view->rect());
    QGridLayout *layout = new QGridLayout(widget);
    layout->addWidget(view, 0, Qt::AlignBottom | Qt::AlignHCenter);
}
void radialPolarChart::setRectAera(QRectF rc, int width)
{
    if(m_Rectframe != nullptr){
        return;
    }
    m_Rectframe = new QGraphicsRectItem(this->polarChart);
    // 设置画笔、画刷
    QPen pen = m_Rectframe->pen();
    pen.setWidth(width);
    pen.setColor(Qt::black);
    pen.setStyle (Qt::SolidLine);
    m_Rectframe->setPen(pen);

    // 矩形区域 起点：(50, 50) 宽：100 高：100 100,100,1000,200
    m_Rectframe->setRect(rc);
    m_Rectframe->show();
    drawAnnotations(rc);
}

void radialPolarChart::drawAnnotations(const QRectF &rc)
{
    int x = rc.x();
    int y = rc.y();
    int width = rc.width();
    int height = rc.height();

    int spacing = 30; // 注解之间的间距
    int startX = x + 10; // 起始位置
    int startY = y + 10; // 起始位置

    // 横线代表数据
    QGraphicsSimpleTextItem *textItem1 = new QGraphicsSimpleTextItem("数据");
    textItem1->setFont(QFont("Arial", 10));
    textItem1->setBrush(QColor(0, 0, 0));
    textItem1->setPos(startX + 20, startY);
    polarChartView->scene()->addItem(textItem1);

    QGraphicsPixmapItem *pixmapItem1 = new QGraphicsPixmapItem();
    pixmapItem1->setPixmap(QPixmap("line.png")); // 替换为实际图片路径
    pixmapItem1->setPos(startX, startY);
    polarChartView->scene()->addItem(pixmapItem1);

    // 三角形代表高点
    QGraphicsSimpleTextItem *textItem2 = new QGraphicsSimpleTextItem("高点");
    textItem2->setFont(QFont("Arial", 10));
    textItem2->setBrush(QColor(0, 0, 0));
    textItem2->setPos(startX + 20, startY + spacing);
    polarChartView->scene()->addItem(textItem2);

    QGraphicsPixmapItem *pixmapItem2 = new QGraphicsPixmapItem();
    pixmapItem2->setPixmap(QPixmap("triangle.png")); // 替换为实际图片路径
    pixmapItem2->setPos(startX, startY + spacing);
    polarChartView->scene()->addItem(pixmapItem2);

    // 叉代表低点
    QGraphicsSimpleTextItem *textItem3 = new QGraphicsSimpleTextItem("低点");
    textItem3->setFont(QFont("Arial", 10));
    textItem3->setBrush(QColor(0, 0, 0));
    textItem3->setPos(startX + 20, startY + 2 * spacing);
    polarChartView->scene()->addItem(textItem3);

    QGraphicsPixmapItem *pixmapItem3 = new QGraphicsPixmapItem();
    pixmapItem3->setPixmap(QPixmap("square.png")); // 替换为实际图片路径
    pixmapItem3->setPos(startX, startY + 2 * spacing);
    polarChartView->scene()->addItem(pixmapItem3);

    // 五角星代表偏心
    QGraphicsSimpleTextItem *textItem4 = new QGraphicsSimpleTextItem("偏心");
    textItem4->setFont(QFont("Arial", 10));
    textItem4->setBrush(QColor(0, 0, 0));
    textItem4->setPos(startX + 20, startY + 3 * spacing);
    polarChartView->scene()->addItem(textItem4);

    QGraphicsPixmapItem *pixmapItem4 = new QGraphicsPixmapItem();
    pixmapItem4->setPixmap(QPixmap("star.png")); // 替换为实际图片路径
    pixmapItem4->setPos(startX, startY + 3 * spacing);
    polarChartView->scene()->addItem(pixmapItem4);

    // 加号代表倾斜
    QGraphicsSimpleTextItem *textItem5 = new QGraphicsSimpleTextItem("倾斜");
    textItem5->setFont(QFont("Arial", 10));
    textItem5->setBrush(QColor(0, 0, 0));
    textItem5->setPos(startX + 20, startY + 4 * spacing);
    polarChartView->scene()->addItem(textItem5);

    QGraphicsPixmapItem *pixmapItem5 = new QGraphicsPixmapItem();
    pixmapItem5->setPixmap(QPixmap("plus.png")); // 替换为实际图片路径
    pixmapItem5->setPos(startX, startY + 4 * spacing);
    polarChartView->scene()->addItem(pixmapItem5);

    // 实心圆代表分布点
    QGraphicsSimpleTextItem *textItem6 = new QGraphicsSimpleTextItem("数据分布点");
    textItem6->setFont(QFont("Arial", 10));
    textItem6->setBrush(QColor(0, 0, 0));
    textItem6->setPos(startX + 20, startY + 5 * spacing);
    polarChartView->scene()->addItem(textItem6);

    QGraphicsPixmapItem *pixmapItem6 = new QGraphicsPixmapItem();
    pixmapItem6->setPixmap(QPixmap("circle.png")); // 替换为实际图片路径
    pixmapItem6->setPos(startX, startY + 5 * spacing);
    polarChartView->scene()->addItem(pixmapItem6);

}

//void radialPolarChart::iinitPolarChart()
//{
//        const qreal angularMin = 0;
//        const qreal angularMax = 100;
//        const qreal radialMin = 0;
//        const qreal radialMax = 100;

//        chartView = new ChartView();

//        QScatterSeries *series1 = new QScatterSeries();
//        series1->setName("scatter");
//        for (int i = angularMin; i <= angularMax; i += 10){
//    //		series1->append(i, (i / radialMax) * radialMax);

//            double angle = i * (360.0 / 360.0); // 转换为弧度
//            series1->append(angle, 50.0); // 半径为 1.0
//        }

//        QSplineSeries *series2 = new QSplineSeries();
//        series2->setName("spline");
//        for (int i = angularMin; i <= angularMax; i += 10){
//    //		series2->append(i, (i / radialMax) * radialMax);

//            double angle = i * (360.0 / 360.0); // 转换为弧度
//            series2->append(angle, 50.0); // 半径为 1.0
//        }

//        //![1]
//        chart = new QPolarChart();
//        //![1]
//        chart->addSeries(series1);
//        chart->addSeries(series2);

//        //![2]
//        QValueAxis *angularAxis = new QValueAxis();
//        angularAxis->setTickCount(9); // First and last ticks are co-located on 0/360 angle.
//        angularAxis->setLabelFormat("%.1f");
//        angularAxis->setShadesVisible(true);
//        angularAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
//        chart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);

//        QValueAxis *radialAxis = new QValueAxis();
//        radialAxis->setTickCount(9);
//        radialAxis->setLabelFormat("%d");
//        chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);
//        //![2]

//        series1->attachAxis(radialAxis);
//        series1->attachAxis(angularAxis);
//        series2->attachAxis(radialAxis);
//        series2->attachAxis(angularAxis);

//        //这个是显示坐标值的
//        series2->setPointsVisible(true);
//        series2->setPointLabelsVisible();

//        //这个是显示坐标值的
//    //	series1->setPointLabelsFormat("[@xPoint, @yPoint]");
//        series2->setPointLabelsFormat("[@xPoint, @yPoint]");

//        radialAxis->setRange(radialMin, radialMax);
//        angularAxis->setRange(angularMin, angularMax);

//        chartView->setChart(chart);
//        chartView->setRenderHint(QPainter::Antialiasing);


//        QList<QPointF> pos;
//        pos.append(QPointF(0,50));
//        pos.append(QPointF(80,50));
//        pos.append(QPointF(70,50));
//        pos.append(QPointF(10,10));
//        pos.append(QPointF(50,50));

//        chartView->FlagPosList = pos;


//        // 设置渲染提示
//        chartView->setRenderHint(QPainter::HighQualityAntialiasing);

//        // 设置布局
//        chart->setGeometry(chartView->rect());
//        QGridLayout *layout = new QGridLayout(widget);
//        layout->addWidget(chartView, 0, Qt::AlignBottom | Qt::AlignHCenter);

//}

