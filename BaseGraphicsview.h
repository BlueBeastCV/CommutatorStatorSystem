#ifndef BASEGRAPHICSVIEW_H
#define BASEGRAPHICSVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QLabel>
#include "ImageWidget.h"

class BaseGraphicsView : public QGraphicsView
{
        Q_OBJECT
    public:
        explicit BaseGraphicsView(QWidget *parent = nullptr);
        void init();
        void loadPixmap(QPixmap* pixmap);
        void fitScreen();
        void setShowCrossFlag(bool flag){showCrossFlag = flag;}
        void setAxisZeroFlag(bool flag){AxisZeroIsCenter = flag;}
        void clearAllItem();
    protected:
        virtual void mousePressEvent(QMouseEvent *event) override;
        virtual void resizeEvent(QResizeEvent *event) override;
        virtual void drawForeground(QPainter *painter, const QRectF &rect) override;
//        virtual void wheelEvent(QWheelEvent *event) override;
        virtual void mouseMoveEvent(QMouseEvent *event) override;
        virtual void drawBackground(QPainter* painter, const QRect& rect);


    signals:
        void sendImgInfo(QColor pixelColor, QPoint posPoint);

    private:
        QGraphicsScene  qgraphicsScene;
        ImageWidget * m_Image = nullptr;
        int viewHeight;
        int viewWidth;
        bool AxisZeroIsCenter = true;//默认中心显示
        bool showCrossFlag = false;//默认显示十字中心线



};

#endif // BASEGRAPHICSVIEW_H
