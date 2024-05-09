#include "BaseGraphicsview.h"

BaseGraphicsView::BaseGraphicsView(QWidget* parent)
    : QGraphicsView{parent}
{
    // 隐藏水平/竖直滚动条
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //setDragMode(QGraphicsView::RubberBandDrag);
    // 设置场景范围 尺寸改变事件动态获取当前窗口大小
//	setSceneRect(INT_MIN / 2, INT_MIN / 2, INT_MAX, INT_MAX);
    // 反锯齿
    setRenderHints(QPainter::Antialiasing);
    init();
    setMouseTracking(true);//不按下鼠标 也会触发 mouseMoveEvent
}

void BaseGraphicsView::init()
{
    this->setScene(&qgraphicsScene);//Sets the current scene to scene. If scene is already being viewed, this function does nothing.
    this->setFocus();//将界面的焦点设置到当前Graphics View控件
    // 创建坐标显示标签

}

void BaseGraphicsView::loadPixmap(QPixmap *pixmap)
{
//	 QList<QGraphicsItem *> items = qgraphicsScene.items();
//	 qDebug() << "QGraphicsItem size:" << items.size();
     if(m_Image == nullptr){
         m_Image = new ImageWidget(pixmap, AxisZeroIsCenter);
         connect(m_Image, &ImageWidget::sendImgInfo, this, &BaseGraphicsView::sendImgInfo);
         qreal scale = m_Image->getScaleValue();
         m_Image->setQGraphicsViewWH(viewWidth, viewHeight, scale);//将界面控件Graphics View的width和height传进类m_Image中
         qgraphicsScene.addItem(m_Image);//将QGraphicsItem类对象放进QGraphicsScene中
     }else{
         m_Image->loadPix(pixmap, AxisZeroIsCenter);
         qreal scale = m_Image->getScaleValue();
         m_Image->setQGraphicsViewWH(viewWidth, viewHeight, scale);//将界面控件Graphics View的width和height传进类m_Image中
         qgraphicsScene.update();
     }
     //更新显示窗口
     if(AxisZeroIsCenter == true){
         qgraphicsScene.setSceneRect(QRectF(-viewWidth/2, -viewHeight/2, viewWidth, viewHeight));//场景坐标中间
         //qgraphicsScene.addRect(-viewWidth/2, -viewHeight/2, viewWidth, viewHeight, QPen(Qt::red)); //红色方框标明场景区域
     }else{
         qgraphicsScene.setSceneRect(QRectF(0,0, viewWidth, viewHeight));//场景坐标左上角
         //qgraphicsScene.addRect(0, 0, viewWidth, viewHeight, QPen(Qt::red)); //红色方框标明场景区域
     }
}

void BaseGraphicsView::fitScreen()
{
    if(m_Image != nullptr){
        m_Image->ResetItemPos();
    }
}

void BaseGraphicsView::clearAllItem()
{
    if(m_Image != nullptr){
        delete m_Image;
        m_Image = nullptr;
    }
    qgraphicsScene.clear();
}

void BaseGraphicsView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
}
//如果有布局是获取不到当前窗口的大小值，只能通过sizeEvent事件来获取当前窗口大小
void BaseGraphicsView::resizeEvent(QResizeEvent *event)
{
    QSize size = event->size();
    viewWidth = size.width();
    viewHeight = size.height();
    if(m_Image != nullptr){
        m_Image->setQGraphicsViewWH(viewWidth, viewHeight);
    }

    if(AxisZeroIsCenter == true){
        qgraphicsScene.setSceneRect(QRectF(-viewWidth/2, -viewHeight/2, viewWidth, viewHeight));//场景坐标中间
        //qgraphicsScene.addRect(-viewWidth/2, -viewHeight/2, viewWidth, viewHeight, QPen(Qt::red)); //红色方框标明场景区域
    }else{
        qgraphicsScene.setSceneRect(QRectF(0,0, viewWidth, viewHeight));//场景坐标左上角
        //qgraphicsScene.addRect(0, 0, viewWidth, viewHeight, QPen(Qt::red)); //红色方框标明场景区域
    }
    QGraphicsView::resizeEvent(event);
}

void BaseGraphicsView::drawForeground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect)
    if(showCrossFlag == true){//显示十字线
        QRectF rt = scene()->sceneRect();  //获取场景大小的绘制面积
        double D = 50;
        QPen _pen;
        _pen.setColor(QColor(255,0,0));
        _pen.setWidth(2);
        painter->setPen(_pen);
        //绘制两条红色十字交叉线
        painter->drawLine(QPointF(rt.center().x()-D, rt.center().y()), QPointF(rt.center().x()+D, rt.center().y()));//绘制直线
        painter->drawLine(QPointF(rt.center().x(), rt.center().y()-D), QPointF(rt.center().x(), rt.center().y()+D));//绘制直线

//			_pen.setColor(QColor(245,240,85));
//			_pen.setWidth(2);
//			painter->setPen(_pen);
//			painter->drawRect(rect);
    }


}

void BaseGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);

}

void BaseGraphicsView::drawBackground(QPainter *painter, const QRect &rect)
{

    //QGraphicsScene::drawBackground(painter,rect);
    QGraphicsView::drawBackground(painter,rect);
//    if(!isShowGrid)
//        return;

    Q_UNUSED(rect);
    QPen pen;

    qreal left=rect.left();
    qreal right=rect.right();
    qreal top=rect.top();
    qreal bottom=rect.bottom();

    //边界值调整
    left = (left/128)*128;
    right = (right/128)*128;
    top = (top/128)*128;
    bottom = (bottom/128)*128;

    pen.setColor(QColor(60,60,60));
    pen.setWidth(0);
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);
    //绘制横线
    //绘制Y轴正半轴
    for(int i=0;i>=top;i-=128)
    {
        painter->drawLine(left,i,right,i);
    }
    //绘制Y轴负半轴
    for(int i=0;i<=bottom;i+=128)
    {
        painter->drawLine(left,i,right,i);
    }

    //绘制竖线
    //绘制X轴正半轴
    for(int i=0;i<=right;i+=128)
    {
        painter->drawLine(i,top,i,bottom);
    }
    //绘制X轴负半轴
    for(int i=0;i>=left;i-=128)
    {
        painter->drawLine(i,top,i,bottom);
    }
}
//中心缩放

//void BaseGraphicsView::wheelEvent(QWheelEvent *event)
//{
//    bool ctrlPressed = (event->modifiers() & Qt::ControlModifier);
//    if (!ctrlPressed) {
//        // Ctrl 键未被按下，不进行缩放操作
//        QWidget::wheelEvent(event);
//        return;
//    }
//    // 获取当前鼠标相对于view的位置;
//    QPointF cursorPoint = event->pos();
//    // 获取当前鼠标相对于scene的位置;
//    QPointF scenePos = this->mapToScene(QPoint(cursorPoint.x(), cursorPoint.y()));

//    // 获取view的宽高;
//    qreal viewWidth = this->viewport()->width();
//    qreal viewHeight = this->viewport()->height();

//    // 获取当前鼠标位置相当于view大小的横纵比例;
//    qreal hScale = cursorPoint.x() / viewWidth;
//    qreal vScale = cursorPoint.y() / viewHeight;

//    // 当前放缩倍数;
//    qreal scaleFactor = this->matrix().m11();
//    int wheelDeltaValue = event->delta();
//    // 向上滚动，放大;
//    if (wheelDeltaValue > 0)
//    {
//        //this->scale(1.2, 1.2);

//    }
//    // 向下滚动，缩小;
//    else
//    {
//        this->scale(1.0 / 1.2, 1.0 / 1.2);
//    }


//    // 将scene坐标转换为放大缩小后的坐标;
// //   QPointF viewPoint = this->matrix().map(scenePos);
//    // 通过滚动条控制view放大缩小后的展示scene的位置;
////    horizontalScrollBar()->setValue(int(viewPoint.x() - viewWidth * hScale));
////    verticalScrollBar()->setValue(int(viewPoint.y() - viewHeight * vScale));
//}

