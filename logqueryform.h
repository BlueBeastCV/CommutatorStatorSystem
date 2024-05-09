#ifndef LOGQUERYFORM_H
#define LOGQUERYFORM_H

#include <QWidget>
#include "dataoper.h"
#include "objectinfo.h"
#include <QtXlsx/xlsxdocument.h>
#include <QtXlsx/xlsxworksheet.h>
namespace Ui {
class LogQueryForm;
}

class LogQueryForm : public QWidget
{
        Q_OBJECT

    public:
        explicit LogQueryForm(User user, QWidget *parent = nullptr);
        ~LogQueryForm();
        void initQuery();

//        void initDatatable();

        void GetTotalRecordCount();
        void GetPageCount();
        void UpdateStatus();
        void RecordQuery(int limitIndex);


    private slots:
        void on_queryBtn_clicked();

//        void on_fristButton_clicked();

//        void on_prevButton_clicked();

//        void on_switchPageButton_clicked();

//        void on_nextButton_clicked();

//        void on_lastButton_clicked();

//        void on_pageRecordCount_currentTextChanged(const QString &arg1);

        void on_export_data_btn_clicked();

        void init_excel();

private:
        Ui::LogQueryForm *ui;
        DataOper oper;

        //数据查询  table 分页处理
        int       currentPage;      //当前页
        int       totalPage;    //总页数
        int       totalRecrodCount;     //总记录数
        int       PageRecordCount;      //每页记录数

        QVector<SingleRecord> LogList;//查询数据结果集
        User currentUser;
        QFile file;
        QXlsx::Document* m_xlsx = nullptr;
        QXlsx::Format headFormat;              //表头样式
        QXlsx::Format cellFormat;              //单元格样式
};

#endif // LOGQUERYFORM_H
