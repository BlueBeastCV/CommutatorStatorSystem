#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include <QtGui>
#include <QPixmap>
#include <QPainter>
#include <QRectF>
#include <QMouseEvent>
#include <QPointF>
#include <QDragEnterEvent>
#include <QGraphicsSceneWheelEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QSpinBox>
#include <QWidgetAction>
#include <QMenu>


//为了使用信号和槽，直接多继承，会导致类型转换出错
class BImageWidget :public QObject, public QGraphicsItem
{
		Q_OBJECT
	public:
		BImageWidget(QGraphicsItem * parent = 0);

};
class ImageWidget :public BImageWidget
{
		Q_OBJECT
	public:
		ImageWidget(QPixmap *originalPix, bool AxisZeroIsCenter_ = true);
		QRectF  boundingRect() const;
		void    paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
		void    wheelEvent(QGraphicsSceneWheelEvent *event);
		void    ResetItemPos();
		void    mousePressEvent(QGraphicsSceneMouseEvent *event);
		void    mouseMoveEvent(QGraphicsSceneMouseEvent *event);
		void    mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
		//void    contextMenuEvent(QGraphicsSceneContextMenuEvent* event);//上下文事件
		void    hoverMoveEvent(QGraphicsSceneHoverEvent *event);
		qreal   getScaleValue() const;
		void    setQGraphicsViewWH(int nwidth, int nheight, qreal scale = -100);
		void    loadPix(QPixmap *originalPix, bool AxisZeroIsCenter_ = true);

	signals:
		void sendImgInfo(QColor pixelColor, QPoint posPoint);

	private:
		qreal       m_scaleValue;
		qreal       m_scaleDafault;
		QPixmap     m_pix;//原始图像
		QImage      qimg;//原始图像
		int         m_zoomState;
		bool        m_isMove;
		QPointF     m_startPos;
		QPointF     m_point;
		bool        AxisZeroIsCenter = true;//默认中心显示
};
#endif // IMAGEWIDGET_H
