#include "logqueryform.h"
#include "ui_logqueryform.h"
#include <QMetaEnum>

LogQueryForm::LogQueryForm(User user, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogQueryForm)
{
    ui->setupUi(this);

    myHelper::FormInCenter(this);//窗体居中显示
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    ui->titleWidget->set_lab_Title("数据查询");
    ui->titleWidget->setShowMaxBtn(true);
//    ui->titleWidget->setShowMinBtn(true);
//    ui->titleWidget->setShowCloseBtn(true);

//	ui->result_tableWidget->setColumnWidth(0, 50);
//	ui->result_tableWidget->setColumnWidth(1, 700);

    currentUser = user;
    initQuery();
}

LogQueryForm::~LogQueryForm()
{
    delete ui;
}

void LogQueryForm::initQuery()
{
//    QStringList BatchNumList;
//    oper.getBatchNumList(BatchNumList);
//    ui->batchNumList->clear();
//    ui->batchNumList->addItem("");
//    ui->batchNumList->addItems(BatchNumList);

//    QStringList TypeList;
//    oper.getTypeList(TypeList);
//    ui->TypeList->clear();
//    ui->TypeList->addItem("");
//    ui->TypeList->addItems(TypeList);


    QDate date = QDate::currentDate();   //获取当前日期
    ui->startTime->setDate(date);
    ui->endTime->setDate(date);
    ui->startTime->setDisplayFormat("yyyy-MM-dd");
    ui->endTime->setDisplayFormat("yyyy-MM-dd");
//	ui->result_tableWidget->verticalHeader()->hide();


}
//日志查询
void LogQueryForm::on_queryBtn_clicked()
{
//    initDatatable();
}

//初始化加载数据
//void LogQueryForm::initDatatable()
//{
//    currentPage = 1;
//    GetTotalRecordCount();
//    GetPageCount();
//    UpdateStatus();
//    RecordQuery(currentPage-1);
//}
//总记录数
void LogQueryForm::GetTotalRecordCount()
{
//    QString startTime = ui->startTime->text();
//    QString endTime = ui->endTime->text();
//    SingleRecord log;
//    log.batchNum = ui->batchNumList->currentText();
//    log.type = ui->TypeList->currentText();

//    oper.getLogTotalRecordCountByCondition(totalRecrodCount, log, startTime, endTime);
}
//得到页数
//void LogQueryForm::GetPageCount()
//{
//    PageRecordCount = ui->pageRecordCount->currentText().toInt();
//    totalPage = (totalRecrodCount % PageRecordCount == 0) ? (totalRecrodCount / PageRecordCount) : (totalRecrodCount / PageRecordCount + 1);
//}
//更新按钮状态
//void LogQueryForm::UpdateStatus()
//{
//    ui->currentPage->setText(QString::number(currentPage));
//    ui->totalPage->setText(QString::number(totalPage));
//    if(currentPage == 1){
//        ui->prevButton->setEnabled(false);
//        ui->nextButton->setEnabled(true);
//    }else if(currentPage == totalPage){
//        ui->prevButton->setEnabled(true);
//        ui->nextButton->setEnabled(false);
//    }else{
//        ui->prevButton->setEnabled(true);
//        ui->nextButton->setEnabled(true);
//    }
//}
//查询数据
//void LogQueryForm::RecordQuery(int limitIndex)
//{
//    Q_UNUSED(limitIndex);
//    QString startTime = ui->startTime->text();
//    QString endTime = ui->endTime->text();
//    SingleRecord log;
//    log.batchNum = ui->batchNumList->currentText();
//    log.type = ui->TypeList->currentText();

//    uint stime = QDateTime::fromString(startTime, "yyyy-MM-dd").toTime_t();
//    uint etime = QDateTime::fromString(endTime, "yyyy-MM-dd").toTime_t();
//    int end_ = stime - etime;
//    if(end_ > 0 ){
//        myHelper::ShowMessageBoxInfo("结束日期小于开始日期，重新选择日期");
//        return;
//    }
//    LogList.clear();
//    int OK = oper.getLogDataByCondition(LogList, log, startTime, endTime, limitIndex, PageRecordCount);
//    if(OK == 2){
//        myHelper::ShowMessageBoxInfo("查询失败");
//        return;
//    }
//    int size = LogList.size();
//    if(size == 0){
//        myHelper::ShowMessageBoxInfo("当前条件下没有数据");
//        return;
//    }
//    ui->result_tableWidget->setRowCount(size);

//    for(int i = 0; i < size; ++i){
//        for(int j = 0; j < ui->result_tableWidget->columnCount(); ++j){//列循环
//            QTableWidgetItem *item = new QTableWidgetItem();
//            item->setFlags(item->flags() & 33);//设置列不可编辑
//            item->setTextAlignment(Qt::AlignCenter);
//            if(j == 0){//大径
//                item->setText(QString::number(LogList.at(i).bigDiameter,'f',3));
//            }
//            if(j == 1){//中径
//                item->setText(QString::number(LogList.at(i).effective_diameter,'f',3));
//            }
//            if(j == 2){//小径
//                item->setText(QString::number(LogList.at(i).minor_diameter,'f',3));
//            }
//            if(j == 3){//螺距
//                item->setText(QString::number(LogList.at(i).threadPitch,'f',3));
//            }
//            if(j == 4){//螺纹角
//                item->setText(QString::number(LogList.at(i).Angle,'f',3));
//            }
//            if(j == 5){//扳手孔1
//                item->setText(QString::number(LogList.at(i).driving_hole_radius_1,'f',3));
//            }
//            if(j == 6){//扳手孔2
//                item->setText(QString::number(LogList.at(i).driving_hole_radius_2,'f',3));
//            }
//            if(j == 7){//扳手孔圆心距
//                item->setText(QString::number(LogList.at(i).driving_hole_dist,'f',3));
//            }
//            if(j == 8){//内圆半径
//                item->setText(QString::number(LogList.at(i).inner_radius,'f',3));
//            }
//            if(j == 9){//总高
//                item->setText(QString::number(LogList.at(i).height,'f',3));
//            }
//            if(j == 10){//颈部宽度
//                item->setText(QString::number(LogList.at(i).neck_width,'f',3));
//            }
//            if(j == 11){//台阶高度
//                item->setText(QString::number(LogList.at(i).step_height,'f',3));
//            }
//            if(j == 12){//是否合格
//                switch (LogList.at(i).grade){
//                case 1:{
//                    item->setText(QString("合格"));
//                    break;
//                }
//                case 2: {
//                    item->setText(QString("不合格"));
//                    break;
//                }
//                case 3:{
//                    item->setText(QString("返工"));
//                    break;
//                }}
////                item->setText(LogList.at(i).grade);
//            }
//            ui->result_tableWidget->setItem(i, j, item);

//        }
//    }
//}
//首页
//void LogQueryForm::on_fristButton_clicked()
//{
//    currentPage = 1;
//    GetTotalRecordCount();
//    GetPageCount();
//    UpdateStatus();
//    RecordQuery(currentPage-1);
//}
////上一页
//void LogQueryForm::on_prevButton_clicked()
//{
//    GetTotalRecordCount();
//    GetPageCount();
//    if(totalPage == 1){
//        return;
//    }
//    int limitIndex = (currentPage - 2) * PageRecordCount;
//    RecordQuery(limitIndex);
//    currentPage -= 1;
//    UpdateStatus();
//}
////跳转
//void LogQueryForm::on_switchPageButton_clicked()
//{
//    GetTotalRecordCount();
//    GetPageCount();
//    //得到输入字符串
//    QString szText = ui->switchPageLineEdit->text();
//    //数字正则表达式
//    QRegExp regExp("-?[0-9]*");
//    //判断是否为数字
//    if(!regExp.exactMatch(szText))
//    {
//        myHelper::ShowMessageBoxInfo("请输入数字");
//        ui->switchPageLineEdit->clear();
//        return;
//    }
//    //是否为空
//    if(szText.isEmpty())
//    {
//        myHelper::ShowMessageBoxInfo("请输入跳转页面");
//        ui->switchPageLineEdit->clear();
//        return;
//    }
//    //得到页数
//    int pageIndex = szText.toInt();
//    //判断是否有指定页
//    if(pageIndex > totalPage || pageIndex < 1)
//    {
//        myHelper::ShowMessageBoxInfo("没有指定的页面，请重新输入");
//        ui->switchPageLineEdit->clear();
//        return;
//    }
//    //得到查询起始行号
//    int limitIndex = (pageIndex-1) * PageRecordCount;
//    //记录查询
//    RecordQuery(limitIndex);
//    //设置当前页
//    currentPage = pageIndex;
//    //刷新状态
//    UpdateStatus();
//}
////下一页
//void LogQueryForm::on_nextButton_clicked()
//{
//    GetTotalRecordCount();
//    GetPageCount();
//    if(totalPage == 1){
//        return;
//    }
//    int limitIndex = currentPage * PageRecordCount;
//    RecordQuery(limitIndex);
//    currentPage += 1;
//    UpdateStatus();
//}
////末页
//void LogQueryForm::on_lastButton_clicked()
//{
//    GetTotalRecordCount();
//    GetPageCount();
//    int limitIndex = (totalPage-1) * PageRecordCount;
//    currentPage = totalPage;
//    UpdateStatus();
//    RecordQuery(limitIndex);
//}
////改变每页数量事件
//void LogQueryForm::on_pageRecordCount_currentTextChanged(const QString &arg1)
//{
//    Q_UNUSED(arg1);
//    initDatatable();
//}


void LogQueryForm::on_export_data_btn_clicked()
{
//    if(LogList.empty()){
//        myHelper::ShowMessageBoxInfo("查询值未空！");
//        return;
//    }
//    QString currentDateTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
//    QString dir = myHelper::readSettings("path/excelPath").toString();
//    QString fileName = QString("%1/%2.xlsx").arg(dir).arg(currentDateTime);
//    qDebug() << " excel fileName " << fileName;
//    file.setFileName(fileName);
//    if(m_xlsx != nullptr){
//        delete m_xlsx;
//        m_xlsx = nullptr;
//    }
//    m_xlsx = new QXlsx::Document();//清空数据，不然会一直有缓存的xlsx数据
//    if(file.isOpen() == false){
//        if(!file.open(QIODevice::Append | QIODevice::Text)){
//            qlog("writeExcel打开excel文件失败");
//            return;
//        }
//    }
//    m_xlsx->write(1, 1,  "大径(mm)", headFormat);
//    m_xlsx->write(1, 2,  "中径(mm)", headFormat);
//    m_xlsx->write(1, 3,  "小径(mm)", headFormat);
//    m_xlsx->write(1, 4,  "螺距(mm)", headFormat);
//    m_xlsx->write(1, 5,  "螺纹角(mm)", headFormat);
//    m_xlsx->write(1, 6,  "扳手孔半径1(mm)", headFormat);
//    m_xlsx->write(1, 7,  "扳手孔半径2(mm)", headFormat);
//    m_xlsx->write(1, 8,  "扳手孔距离(mm)", headFormat);
//    m_xlsx->write(1, 9,  "内圆半径(mm)", headFormat);
//    m_xlsx->write(1, 10, "高度(mm)", headFormat);
//    m_xlsx->write(1, 11, "颈部宽度(mm)", headFormat);
//    m_xlsx->write(1, 12, "台阶高度(mm)", headFormat);
//    m_xlsx->write(1, 13, "结果", headFormat);
//    int size = LogList.size();
//    for(int i = 1; i < size; ++i)
//    {
//        m_xlsx->write(i + 1, 1,  LogList.at(i).bigDiameter, cellFormat);
//        m_xlsx->write(i + 1, 2,  LogList.at(i).effective_diameter, cellFormat);
//        m_xlsx->write(i + 1, 3,  LogList.at(i).minor_diameter, cellFormat);
//        m_xlsx->write(i + 1, 4,  LogList.at(i).threadPitch, cellFormat);
//        m_xlsx->write(i + 1, 5,  LogList.at(i).Angle, cellFormat);
//        m_xlsx->write(i + 1, 6,  LogList.at(i).driving_hole_radius_1, cellFormat);
//        m_xlsx->write(i + 1, 7,  LogList.at(i).driving_hole_radius_2, cellFormat);
//        m_xlsx->write(i + 1, 8,  LogList.at(i).driving_hole_dist, cellFormat);
//        m_xlsx->write(i + 1, 9,  LogList.at(i).inner_radius, cellFormat);
//        m_xlsx->write(i + 1, 10,  LogList.at(i).height, cellFormat);
//        m_xlsx->write(i + 1, 11,  LogList.at(i).neck_width, cellFormat);
//        m_xlsx->write(i + 1, 12,  LogList.at(i).step_height, cellFormat);

//        switch (LogList.at(i).grade){
//        case 1:{
//            m_xlsx->write(i + 1, 13, QString("合格") , cellFormat);
//            break;
//        }
//        case 2: {
//            m_xlsx->write(i + 1, 13, QString("不合格") , cellFormat);
//            break;
//        }
//        case 3:{
//            m_xlsx->write(i + 1, 13, QString("返工") , cellFormat);
//            break;
//        }}
//    }

//    if(m_xlsx->saveAs(fileName) == true){
//        qlog("写入excel文件成功");
//        myHelper::ShowMessageBoxInfo("写入excel文件成功");
//    }else{
//        qlog("写入excel文件失败");
//        myHelper::ShowMessageBoxInfo("写入excel文件失败");
//    }
//    file.close();
}

void LogQueryForm::init_excel()
{
//    m_xlsx->write(1, 1,  "大径(mm)", headFormat);
//    m_xlsx->write(1, 2,  "中径(mm)", headFormat);
//    m_xlsx->write(1, 3,  "小径(mm)", headFormat);
//    m_xlsx->write(1, 4,  "螺距(mm)", headFormat);
//    m_xlsx->write(1, 5,  "螺纹角(mm)", headFormat);
//    m_xlsx->write(1, 6,  "扳手孔半径1(mm)", headFormat);
//    m_xlsx->write(1, 7,  "扳手孔半径2(mm)", headFormat);
//    m_xlsx->write(1, 8,  "扳手孔距离(mm)", headFormat);
//    m_xlsx->write(1, 9,  "内圆半径(mm)", headFormat);
//    m_xlsx->write(1, 10, "高度(mm)", headFormat);
//    m_xlsx->write(1, 11, "颈部宽度(mm)", headFormat);
//    m_xlsx->write(1, 12, "台阶高度(mm)", headFormat);
//    m_xlsx->write(1, 13, "结果", headFormat);


}
