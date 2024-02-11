#include "widget.h"
#include "ui_widget.h"
#include <QTimer>
#include <QTime>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    // 隐藏最大化按钮
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    // UI初始化
    ui->setupUi(this);
    // 默认波特率115200
    ui->baudbox->setCurrentIndex(2);
    // 默认停止位是1
    ui->stopbox->setCurrentIndex(1);
    // 初始化QChart的实例
    chart = new QChart();
    Chart_Init(1);
    serialPort = new QSerialPort(this);

    // 接收数据并处理的新线程
    dataProcessor = new DataProcessor();
    dataProcessorThread = new QThread(this);
    dataProcessor->moveToThread(dataProcessorThread);
    dataProcessorThread->start();
    connect(dataProcessor, &DataProcessor::updateChart, this, &Widget::onUpdateChart);
    connect(this, &Widget::newDataAvailable, dataProcessor, &DataProcessor::processData);

    // 接收定时器
    timerrecv = new QTimer();
    connect(timerrecv, SIGNAL(timeout()), this, SLOT(serialPortReadyRead_Slot()));

    // 发送定时器
    timersend = new QTimer();
    connect(timersend, SIGNAL(timeout()), this, SLOT(on_sendDataFile_clicked()));
    // 定时发送
    connect(ui->timesendcheckBox,SIGNAL(stateChanged(int)),this,SLOT(TimeSendReady(int)));
    // 发送数据显示格式切换
    connect(ui->hexsendcheckBox,SIGNAL(stateChanged(int)),this,SLOT(SendDataHex(int)));

    // Arial,字体大小16，Normal：正常，Bold 粗体，Black：黑体，Light：高亮
    QFont font("Time New Romance", 12, QFont::Normal);
    setWindowTitle(" 串口调试助手 - Telson ");

    // 断开按键关使能
    ui->closeUart->setEnabled(false);
    ui->RecvTextEdit->setFont(font);
    ui->SendTextEdit->setFont(font);
    // 清空接收按钮
    connect(ui->clearRecvButton, SIGNAL(clicked()), this, SLOT(ClearRecvButton()));
    // 保存数据按钮
    connect(ui->saveRecvDataFile, SIGNAL(clicked()), this, SLOT(SaveRecvDataFile()));

    // 主题选项
    populateThemeBox();
    // 初始化主题颜色
    QPalette pal = qApp->palette();
    pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
    pal.setColor(QPalette::WindowText, QRgb(0x404044));
    qApp->setPalette(pal);
    // 更新主题
    updateUI();

}

Widget::~Widget()
{
    dataProcessorThread->quit();
    dataProcessorThread->wait();
    delete ui;
}


void Widget::populateThemeBox()
{
    // 添加主题选项
    ui->themeComboBox->addItem("Light", QChart::ChartThemeLight);
    ui->themeComboBox->addItem("Dark", QChart::ChartThemeDark);
    ui->themeComboBox->addItem("Brown Sand", QChart::ChartThemeBrownSand);
    ui->themeComboBox->addItem("Blue NCS", QChart::ChartThemeBlueNcs);
    ui->themeComboBox->addItem("High Contrast", QChart::ChartThemeHighContrast);
    ui->themeComboBox->addItem("Blue Icy", QChart::ChartThemeBlueIcy);
}

/*
    函   数：updateUI
    描   述：更新Ui的主题和Chart主题
    输   入：无
    输   出：无
*/
void Widget::updateUI()
{
    QChart::ChartTheme theme = static_cast<QChart::ChartTheme>(
                ui->themeComboBox->itemData(ui->themeComboBox->currentIndex()).toInt());
    // 图表设置颜色
    chart->setTheme(theme);
    // 设置主题颜色
    QPalette pal = window()->palette();
    if (theme == QChart::ChartThemeLight) {
        pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
        ui->Titellabel->setStyleSheet("color:#404044");
        ui->recvlabel->setStyleSheet("color:#404044");
        ui->sendlabel->setStyleSheet("color:#404044");
    } else if (theme == QChart::ChartThemeDark) {
        pal.setColor(QPalette::Window, QRgb(0x121218));
        pal.setColor(QPalette::WindowText, QRgb(0xd6d6d6));
        ui->Titellabel->setStyleSheet("color:#d6d6d6");
        ui->recvlabel->setStyleSheet("color:#d6d6d6");
        ui->sendlabel->setStyleSheet("color:#d6d6d6");
    } else if (theme == QChart::ChartThemeBrownSand) {
        pal.setColor(QPalette::Window, QRgb(0x9e8965));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
        ui->Titellabel->setStyleSheet("color:#404044");
        ui->recvlabel->setStyleSheet("color:#404044");
        ui->sendlabel->setStyleSheet("color:#404044");
    } else if (theme == QChart::ChartThemeBlueNcs) {
        pal.setColor(QPalette::Window, QRgb(0x018bba));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
        ui->Titellabel->setStyleSheet("color:#404044");
        ui->recvlabel->setStyleSheet("color:#404044");
        ui->sendlabel->setStyleSheet("color:#404044");
    } else if (theme == QChart::ChartThemeHighContrast) {
        pal.setColor(QPalette::Window, QRgb(0xffab03));
        pal.setColor(QPalette::WindowText, QRgb(0x181818));
        ui->Titellabel->setStyleSheet("color:#181818");
        ui->recvlabel->setStyleSheet("color:#181818");
        ui->sendlabel->setStyleSheet("color:#181818");
    } else if (theme == QChart::ChartThemeBlueIcy) {
        pal.setColor(QPalette::Window, QRgb(0xcee7f0));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
        ui->Titellabel->setStyleSheet("color:#404044");
        ui->recvlabel->setStyleSheet("color:#404044");
        ui->sendlabel->setStyleSheet("color:#404044");
    } else {
        pal.setColor(QPalette::Window, QRgb(0xf0f0f0));
        pal.setColor(QPalette::WindowText, QRgb(0x404044));
        ui->Titellabel->setStyleSheet("color:#404044");
        ui->recvlabel->setStyleSheet("color:#404044");
        ui->sendlabel->setStyleSheet("color:#404044");
    }
    window()->setPalette(pal);
}

/*
    函   数：Chart_Init
    描   述：初始化QSplineSeries图表
    输   入：无
    输   出：无
*/
void Widget::Chart_Init(int num)
{
    // 首先移除现有的所有曲线
    for (auto *s : seriesList) {
        chart->removeSeries(s);
        delete s; // 删除曲线对象释放内存
    }
    seriesList.clear();


    for (int i = 0; i < num; ++i) {
        QSplineSeries *series = new QSplineSeries();
        series->setName(QString("曲线 %1").arg(i + 1));
        // 添加数据到曲线...
        chart->addSeries(series);
        seriesList.append(series);
    }

    // 创建坐标轴
    axisX = new QValueAxis();
    chart->addAxis(axisX, Qt::AlignBottom);

    axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);

    // 初始化坐标轴
    axisX->setRange(0, MAX_X);
    axisY->setRange(0, MAX_Y);

    // 把chart显示到窗口上
    ui->GraphicsView->setChart(chart);
    // 设置渲染：抗锯齿，如果不设置那么曲线就显得不平滑
    ui->GraphicsView->setRenderHint(QPainter::Antialiasing);
}

/*
    函   数：on_comboBox_currentIndexChanged
    描   述：检测要绘制的曲线数量是否变更
    输   入：无
    输   出：无
*/
void Widget::on_NumofCurve_currentIndexChanged(int index)
{
    Chart_Change(index+1);
}
/*
    函   数：Chart_Change
    描   述：改变图表绘制的曲线数量
    输   入：无
    输   出：无
*/
void Widget::Chart_Change(int num)
{
    // 首先移除现有的所有曲线
    for (auto *s : seriesList) {
        chart->removeSeries(s);
        delete s; // 删除曲线对象释放内存
    }
    seriesList.clear();


    for (int i = 0; i < num; ++i) {
        QSplineSeries *series = new QSplineSeries();
        series->setName(QString("曲线 %1").arg(i + 1));
        // 添加数据到曲线...
        chart->addSeries(series);
        seriesList.append(series);
    }

    // 将所有绘制的线绑定到轴上
    for (auto *series : seriesList) {
        series->attachAxis(axisX);
        series->attachAxis(axisY);
    }

    // 每次更换绘制的曲线，currentX初始化
    currentX = 0;
    // 初始化轴范围
    axisX->setRange(0, MAX_X);
    axisY->setRange(0, MAX_Y);
}
/*
    函   数：on_OpenUart_clicked
    描   述：打开并设置串口的信号槽函数
    输   入：无
    输   出：无
*/
void Widget::on_openUart_clicked()
{

    serialPort->setPortName(ui->portcomboBox->currentText());
    serialPort->setBaudRate(ui->baudbox->currentText().toInt());            //设置波特率
    QSerialPort::DataBits selectedDataBits = static_cast<QSerialPort::DataBits>(ui->databox->currentText().toInt());
    serialPort->setDataBits(selectedDataBits);                              //设置数据位数  默认8位
    QSerialPort::Parity selectedStopParity = static_cast<QSerialPort::Parity>(ui->nonebox->currentText().toInt());
    serialPort->setParity(selectedStopParity);                           //设置奇偶校验  默认无奇偶
//    qDebug() << "selectedStopParity:" << selectedStopParity;
    QSerialPort::StopBits selectedStopBits = static_cast<QSerialPort::StopBits>(ui->stopbox->currentText().toInt()); // 获取当前选中的停止位值
    serialPort->setStopBits(selectedStopBits);                          //设置停止位    默认无停止
    serialPort->setFlowControl(QSerialPort::NoFlowControl);                 //设置流控制    默认无

//    qDebug() << ui->portcomboBox->currentText();
//    qDebug() << ui->baudbox->currentText().toInt();
//    qDebug() << selectedDataBits;
//    qDebug() << selectedStopParity;
//    qDebug() << selectedStopBits;
    // 打开串口成功
    if(serialPort->open(QIODevice::ReadWrite))
    {
        ui->openUart->setEnabled(false);
        ui->closeUart->setEnabled(true);
        Serial_State = TRUE;
//        QMessageBox::warning(this,tr("提示"),tr("串口连接成功"));
        Recv_Time();
    }
    else
    {
        QMessageBox::warning(this,tr("错误"),tr("串口连接失败"));
    }
}

void Widget::Recv_Time(){
    timerrecv->start(10);
}

/*
    函   数：serialPortReadyRead_Slot
    描   述：接收数据文本框的槽函数
    输   入：无
    输   出：无
*/

void Widget::serialPortReadyRead_Slot()
{

    // 防止线程阻塞崩溃
    qApp->processEvents();

    // 读取串口接收的数据
    TempRe = serialPort->readAll();
    if (!TempRe.isEmpty()) {
        RecvBuff.append(TempRe);
    }
//    qDebug() << RecvBuff;

    if(RecvBuff.contains('\n')){
        QString stringdata;
        ui->RecvTextEdit->setTextColor(QColor(Qt::black));
        // hex显示
        if(ui->recvcheckBox->isChecked())
        {
            stringdata = RecvBuff.toHex(' ').trimmed().toUpper();
        }
        else
        {
           // ascii显示
           stringdata = QString(RecvBuff);
//           qDebug() << RecvBuff;
        }

        // 时间戳按钮
        if (ui->timecheckBox->isChecked())
        {
           stringdata = QString("[%1]:RX -> %2").arg(QTime::currentTime().toString("HH:mm:ss:zzz")).arg(stringdata);
           ui->RecvTextEdit->append(stringdata);
        }
        else
        {
            ui->RecvTextEdit->append(stringdata);
        }

        // 自动绘图
        int linesize = seriesList.size();
        emit newDataAvailable(RecvBuff, linesize); // 发送信号给数据处理线程
    }

}

/*
    函   数：on_CloseUart_clicked
    描   述：关闭串口连接
    输   入：无
    输   出：无
*/
void Widget::on_closeUart_clicked()
{
    timerrecv->stop();
    serialPort->close();
    Serial_State = 0;
    // 连接串口按键使能
    ui->openUart->setEnabled(true);
    // 断开按键关使能
    ui->closeUart->setEnabled(false);
}

/*
    函   数：on_SendDataFile_clicked
    描   述：手动发送数据
    输   入：无
    输   出：无
*/
void Widget::on_sendDataFile_clicked()
{

    if (Serial_State == TRUE)
    {
        // 获取发送框字符
        SendTextStr = ui->SendTextEdit->document()->toPlainText();
        SendTextByte = SendTextStr.toUtf8();
        if (SendTextByte.isEmpty() != true)
        {
            QString strdata;
            if (ui->hexsendcheckBox->isChecked())
            {
                SendTextByte = SendTextByte.fromHex(SendTextByte);
                // 发送hex数据
                serialPort->write(SendTextByte);
                strdata = SendTextByte.toHex(' ').trimmed().toUpper();
            }
            else
            {
                // 发送ascii数据
                serialPort->write(SendTextByte);
            }
            // 时间戳发送
            if (ui->timecheckBox->isChecked())
            {
                // 时间戳颜色
                ui->RecvTextEdit->setTextColor(QColor(Qt::blue));
                ui->RecvTextEdit->append(QString("[%1]:TX -> %2").arg(QTime::currentTime().toString("HH:mm:ss:zzz")).arg(strdata));
            }
            ui->RecvTextEdit->setTextColor(QColor(Qt::black));
            ui->RecvTextEdit->append(strdata);
        }
        else
        {
            QMessageBox::warning(this, "警告", "请在发送编辑框中输入要发送的数据");
        }

    }
    else
    {
        QMessageBox::information(this, "警告", "串口未打开");
    }
}

/*
    函   数：TimeSendReady
    描   述：定时发送信号槽函数
    输   入：无
    输   出：无
*/
void Widget::TimeSendReady(int state)
{
    int settime;

    if (Serial_State == FALSE)
    {
        QMessageBox::information(this, "提示", "串口未打开");
        return;
    }
    // 判断是否有数据
    if (ui->SendTextEdit->document()->isEmpty() == true)
    {
        QMessageBox::warning(this, "警告", "请在发送编辑框中输入要发送的数据");
        return;
    }
    // 判断勾选状态
    if (state == Qt::Checked)
    {
        // 获取设定时间
        settime = ui->timelineEdit->text().toInt();
        if (settime > 0) {
            timersend->start(settime);
        } else {
            QMessageBox::warning(this, "警告", "时间必须大于 0 ");
            return;
        }
        ui->timelineEdit->setEnabled(false);
    }
    else
    {
        // 停止发送
        timersend->stop();
        ui->timelineEdit->setEnabled(true);
    }
}

/*
    函   数：SendDataHex
    描   述：在发送信息框内显示对应的格式
    输   入：无
    输   出：无
*/
void Widget::SendDataHex(int state)
{
    // 获取发送框字符
    SendTextStr = ui->SendTextEdit->document()->toPlainText();
    SendTextByte = SendTextStr.toUtf8();
    if (SendTextStr.isEmpty())
    {
        return;
    }
    // asccii与hex转换
    if (state == Qt::Checked)
    {
        // 转换成16进制数并转换为大写
        SendTextByte = SendTextByte.toHex(' ').toUpper();
        ui->SendTextEdit->document()->setPlainText(SendTextByte);
    }
    else
    {
        // 从QByteArray转换为QString
        SendTextStr = SendTextByte.fromHex(SendTextByte);
        ui->SendTextEdit->document()->setPlainText(SendTextStr);
    }
}

/*
    函   数：ClearRecvButton and SaveRecvDataFile
    描   述：清除数据和保存数据按钮槽函数
    输   入：无
    输   出：无
*/

void Widget::ClearRecvButton()
{
    ui->RecvTextEdit->clear();
    dataRxNumber = 0 ;
    ui->RxNumlabel->setText(QString::number(dataRxNumber));
}

void Widget::SaveRecvDataFile()
{
    QString data;

    data = ui->RecvTextEdit->toPlainText();

    if (data.isEmpty())
    {
        QMessageBox::information(this, "提示", "数据内容为空");
        return;
    }
    QString curPath = QDir::currentPath();            //获取系统当前目录
    QString dlgTitle = "保存文件";                     //对话框标题
    QString filter = "文本文件(*.txt);;所有文件(*.*)";  //文件过滤器
    QString filename = QFileDialog::getSaveFileName(this,dlgTitle,curPath,filter);
    if (filename.isEmpty())
    {
        return;
    }
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        return;
    }
    // 保存文件
    QTextStream stream(&file);
    stream << data;
    file.close();
}

/*
    函   数：on_savePic_clicked
    描   述：更新绘图内容
    输   入：无
    输   出：无
*/
void Widget::on_savePic_clicked()
{
    if (!chart) return; // 确保图表对象有效
    QPixmap pixmap = ui->GraphicsView->grab(); // 捕获图表的当前视图
    QString curPath = QDir::currentPath();            //获取系统当前目录
    QString dlgTitle = "保存图片";                     //对话框标题
    QString filter = "图片文件(*.png *.jpg);;所有文件(*.*)";  //文件过滤器
    QString filename = QFileDialog::getSaveFileName(this,dlgTitle,curPath,filter);
    if (!filename.isEmpty())
    {
        if (!pixmap.save(filename))
        {
            QMessageBox::warning(this, "保存图片", "保存图片失败");
        }
    }
}

/*
    函   数：DrawLine
    描   述：更新绘图内容
    输   入：无
    输   出：无
*/
void Widget::onUpdateChart(const QVector<double> &data,const QByteArray &leftbuff, const int min)
{

    for (int i = 0; i < min; ++i) { // 遍历所有值
            QSplineSeries *series = seriesList.at(i); // 从seriesList中取出对应的曲线
            if (series->count() >= MAX_X) {
                series->remove(0); // 移除系列中的第一个数据点
            }
            series->append(currentX, data[i]); // 添加新的数据点
            if (series->count() == MAX_X) {
                axisX->setRange(currentX - MAX_X + 1, currentX + 1); // 更新X轴范围
            }
//            qDebug() << data[i];
            if (data[i] > maxYValue) {
                maxYValue = data[i];
            }
            if (data[i] < minYValue) {
                minYValue = data[i];
            }
            // 为Y轴的最大值和最小值留出一定的边界
            double upperBound = maxYValue + (maxYValue - minYValue) * 0.1; // 上限增加10%的边界
            double lowerBound = minYValue - (maxYValue - minYValue) * 0.1; // 下限减少10%的边界

            // 更新Y轴范围
            axisY->setRange(lowerBound, upperBound);
    }
    currentX++; // 递增currentX
    QByteArray tempBuff = RecvBuff;
    RecvBuff = leftbuff; // 保留未完成的部分，加强数据的连贯性
    dataRxNumber += (unsigned int)(tempBuff.length()-RecvBuff.length());
    ui->RxNumlabel->setText(QString::number(dataRxNumber));
}

/*
    函   数：on_showX_toggled
    描   述：显示chart的X轴
    输   入：无
    输   出：无
*/
void Widget::on_showX_stateChanged(int checked)
{
    // 隐藏X坐标轴
    if (checked == Qt::Checked){
        axisX->setVisible(false);
    }else {
        axisX->setVisible(true);
    }
}

void Widget::on_showY_stateChanged(int checked)
{
    // 隐藏Y坐标轴
    if (checked == Qt::Checked){
        axisY->setVisible(false);
    }else {
        axisY->setVisible(true);
    }
}

/*
    函   on_showGrid_toggled and on_showLegend_toggled
    描   述：显示网格和图例
    输   入：无
    输   出：无
*/
void Widget::on_showGrid_stateChanged(int checked)
{
    // 设置网格不显示
    if (checked == Qt::Checked){
        axisX->setGridLineVisible(false);
        axisY->setGridLineVisible(false);
    }else {
        axisX->setGridLineVisible(true);
        axisY->setGridLineVisible(true);
    }
}

void Widget::on_showLegend_stateChanged(int checked)
{
    // 隐藏曲线的图例
    if (checked == Qt::Checked){
        chart->legend()->hide();
    }else {
        chart->legend()->show();
    }
}
