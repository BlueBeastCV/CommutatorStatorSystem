#include "ImageWidget.h"

#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QGraphicsSceneDragDropEvent>
#include <QDrag>
#include <math.h>

BImageWidget::BImageWidget(QGraphicsItem * parent):QGraphicsItem(parent)
{

}

ImageWidget::ImageWidget(QPixmap *originalPix, bool AxisZeroIsCenter_): BImageWidget()
{
	m_pix = *originalPix;
    m_scaleValue = 0;
    m_scaleDafault = 0;
    m_isMove = false;
	this->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable|
				   QGraphicsItem::ItemSendsGeometryChanges);
	AxisZeroIsCenter = AxisZeroIsCenter_;
	this->setAcceptHoverEvents(true);
	qimg = m_pix.toImage();

}

QRectF ImageWidget::boundingRect() const
{
	if(AxisZeroIsCenter == true){
		//图片坐标显示中间
		return QRectF(-m_pix.width() / 2, -m_pix.height() / 2, m_pix.width(), m_pix.height());
	}else{
		//图片坐标显示左上角
		return QRectF(0, 0, m_pix.width(), m_pix.height());
	}
}

void ImageWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
	if(AxisZeroIsCenter == true){
		//图片坐标显示中间
		painter->drawPixmap(-m_pix.width() / 2, -m_pix.height() / 2, m_pix);
	}else{
		//图片坐标显示左上角
		painter->drawPixmap(0, 0, m_pix);
	}
}


void ImageWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button()== Qt::LeftButton)
    {
        m_startPos = event->pos();//鼠标左击时，获取当前鼠标在图片中的坐标，
        m_isMove = true;//标记鼠标左键被按下
    }
    else if(event->button() == Qt::RightButton)
    {
        ResetItemPos();//右击鼠标重置大小
    }

}

void ImageWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(m_isMove)
    {
        QPointF point = (event->pos() - m_startPos)*m_scaleValue;
		moveBy(point.x(), point.y());
	}

}

void ImageWidget::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
	QPointF newPos = event->pos();
//	qDebug() << "newPos:" << newPos;
	QPoint axisPoint = newPos.toPoint();
	QColor pixelColor;
	//如果居中显示需要转换坐标-左上角
	if(AxisZeroIsCenter == true){
		QPoint newP(axisPoint.x() + m_pix.width() / 2, axisPoint.y() + m_pix.height() / 2);
		pixelColor = qimg.pixelColor(newP);
		axisPoint = newP;
	}else{
		QPoint newP2(axisPoint.x(), axisPoint.y());
		pixelColor = qimg.pixelColor(newP2);
		axisPoint = newP2;
	}
	emit sendImgInfo(pixelColor, axisPoint);
}

void ImageWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    m_isMove = false;//标记鼠标左键已经抬起
}

void ImageWidget::wheelEvent(QGraphicsSceneWheelEvent *event)//鼠标滚轮事件
{
    if((event->delta() > 0)&&(m_scaleValue >= 50))//最大放大到原始图像的50倍
    {
        return;
    }
    else if((event->delta() < 0)&&(m_scaleValue <= m_scaleDafault))//图像缩小到自适应大小之后就不继续缩小
    {
        ResetItemPos();//重置图片大小和位置，使之自适应控件窗口大小
	} else {
        qreal qrealOriginScale = m_scaleValue;
        if(event->delta() > 0)//鼠标滚轮向前滚动
        {
            m_scaleValue*=1.1;//每次放大10%
        }
        else
        {
            m_scaleValue*=0.9;//每次缩小10%
        }
        setScale(m_scaleValue);
        if(event->delta() > 0)
        {
            moveBy(-event->pos().x()*qrealOriginScale*0.1, -event->pos().y()*qrealOriginScale*0.1);//使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
        }
        else
        {
            moveBy(event->pos().x()*qrealOriginScale*0.1, event->pos().y()*qrealOriginScale*0.1);//使图片缩放的效果看起来像是以鼠标所在点为中心进行缩放的
        }
    }
}
/*
void ImageWidget::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
//	if ( !this->isSelected() )
//		return;

	QMenu* menu = new QMenu();
	menu->setStyleSheet("QMenu { background-color:rgb(89,87,87); border: 5px solid rgb(235,110,36); }");

	QSpinBox* width_spinBox = new QSpinBox(menu);
	width_spinBox->setStyleSheet("QSpinBox{ width:120px; height:30px; font-size:16px; font-weight:bold; }");
	width_spinBox->setRange(0, 1000);
	width_spinBox->setPrefix("w: ");
	width_spinBox->setSuffix(" mm");
	width_spinBox->setSingleStep(1);
	width_spinBox->setValue(0);
//	connect(width_spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int v){
//		if (m_edge.x() < 0) {
//			m_edge.setX((-1) * v/2);
//		} else {
//			m_edge.setX(v/2);
//		}
//		m_pointList.at(0)->setPoint(m_edge);
//		this->hide();
//		this->update();
//		this->show();
//	});

	QSpinBox* height__spinBox = new QSpinBox(menu);
	height__spinBox->setStyleSheet("QSpinBox{ width:120px; height:30px; font-size:16px; font-weight:bold; }");
	height__spinBox->setRange(0, 1000);
	height__spinBox->setPrefix("h: ");
	height__spinBox->setSuffix(" mm");
	height__spinBox->setSingleStep(1);
	height__spinBox->setValue(0);
//	connect(height__spinBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), [=](int v){
//		if (m_edge.y() < 0) {
//			m_edge.setY((-1) * v/2);
//		} else {
//			m_edge.setY(v/2);
//		}
//		m_pointList.at(0)->setPoint(m_edge);
//		this->hide();
//		this->update();
//		this->show();
//	});

	QWidgetAction* width_widgetAction = new QWidgetAction(menu);
	width_widgetAction->setDefaultWidget(width_spinBox);
	menu->addAction(width_widgetAction);

	QWidgetAction* height_widgetAction = new QWidgetAction(menu);
	height_widgetAction->setDefaultWidget(height__spinBox);
	menu->addAction(height_widgetAction);

	menu->exec(QCursor::pos());
	delete menu;

	QGraphicsItem::contextMenuEvent(event);
}
*/

//将主界面的控件QGraphicsView的width和height传进本类中
//并根据图像的长宽和控件的长宽的比例来使图片缩放到适合控件的大小
void ImageWidget::setQGraphicsViewWH(int nwidth, int nheight, qreal scale)
{
    int nImgWidth = m_pix.width();
    int nImgHeight = m_pix.height();
	qreal temp1 = nwidth*1.0/nImgWidth;
	qreal temp2 = nheight*1.0/nImgHeight;
	if(temp1>temp2) {
        m_scaleDafault = temp2;
	} else {
        m_scaleDafault = temp1;
    }
//	qDebug() << "scale:" << scale << "m_scaleDafault:" << m_scaleDafault;
	if(scale != m_scaleDafault && scale != 0 && scale != -100){//更换图片
		setScale(scale);
		m_scaleValue = scale;
	}else if(scale == -100){//窗口拉伸
		ResetItemPos();
	}else{
		setScale(m_scaleDafault);
		m_scaleValue = m_scaleDafault;
	}



}

void ImageWidget::loadPix(QPixmap* originalPix, bool AxisZeroIsCenter_)
{
	m_pix = *originalPix;
	qimg = m_pix.toImage();
	AxisZeroIsCenter = AxisZeroIsCenter_;
	this->update();
}

void ImageWidget::ResetItemPos()//重置图片位置
{
    m_scaleValue = m_scaleDafault;//缩放比例回到一开始的自适应比例
    setScale(m_scaleDafault);//缩放到一开始的自适应大小
    setPos(0,0);
}

qreal ImageWidget::getScaleValue() const
{
    return m_scaleValue;
}
