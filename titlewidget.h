#ifndef TITLEWIDGET_H
#define TITLEWIDGET_H

#include <QWidget>
#include <QPoint>
#include <QMouseEvent>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

class TitleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TitleWidget(QWidget *parent = nullptr);

    void set_lab_Title(const QString title,QFont font = QFont("Microsoft JhengHei", 12, QFont::Normal)){
        lab_Title->setText(title);
        lab_Title->setFont(font);
    }
    void set_lab_Ico(const QPixmap ico){lab_Ico->setPixmap(ico);}
    void set_title_height(int height);

    void setShowMenuBtn(bool flag);
    void setShowMinBtn(bool flag);
    void setShowMaxBtn(bool flag);
    void setShowCloseBtn(bool flag);
    void setShowLabIco(bool flag);
    void setShowLabTitle(bool flag);
    void setWindowMove(bool flag);

    void defaultMax();//默认最大化

protected:
    void mousePressEvent(QMouseEvent* mEvent);
    void mouseMoveEvent(QMouseEvent* mEvent);
    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *obj, QEvent *event);

private:

    void init();
    QPoint mousePoint;
    bool mousePressed;
    bool max;
    bool showMinBtn;
    bool showMaxBtn;
    bool showMenuBtn;
    bool showCloseBtn;
    bool showLab_Ico;
    bool showLab_Title;
    bool windowIsMove;
    int title_height;
    QLabel *lab_Ico;
    QLabel *lab_Title;
    QPushButton *btnMenu;
    QPushButton *btnMenu_Min;
    QPushButton *btnMenu_Max;
    QPushButton *btnMenu_Close;
    QHBoxLayout *hLay;

};

#endif // TITLEWIDGET_H
