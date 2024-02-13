#ifndef WIDGET_H
#define WIDGET_H

#include <QMainWindow>
#include <QSerialPortInfo>
#include <QString>
#include <QMessageBox>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <QFileInfo>
#include <QSerialPortInfo>
#include <QtSerialPort>
#include <QtCharts>
#include <QChartView>
#include <QSplineSeries>
#include <QValueAxis>
#include "DataProcessor.h"

QT_CHARTS_USE_NAMESPACE

#define MAX_X   300
#define MAX_Y   5
#define TRUE    1
#define FALSE   0

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    // 主题和图表
    void updateUI();
    void Chart_Init();
    void Chart_Change(int);

    // 串口操作
    void on_openUart_clicked();
    void serialPortReadyRead_Slot();
    void on_closeUart_clicked();

    // 发送数据
    void on_sendDataFile_clicked();
    void TimeSendReady(int state);
    void SendDataHex(int state);

    // 数据处理
    void ClearRecvButton();
    void SaveRecvDataFile();

    // 图形绘制
    void onUpdateChart(const QVector<double> &data,const QByteArray &leftbuff, const int min);

    // 创建绘制的曲线数量
    void on_NumofCurve_currentIndexChanged(int);
    // 保存图片
    void on_savePic_clicked();

    // 设置显示XY轴
    void on_showX_stateChanged(int);
    void on_showY_stateChanged(int);

    // 设置显示网格
    void on_showGrid_stateChanged(int);
    // 设置显示图例
    void on_showLegend_stateChanged(int);

    // 接收数据定时器
    void Recv_Time();

signals:
    void newDataAvailable(const QByteArray &data, const int size);

private:
    Ui::Widget *ui;
    // 接收/发送定时器
    QTimer *timerrecv;
    QTimer *timersend;

    // 新线程和数据处理
    QThread *dataProcessorThread;
    DataProcessor *dataProcessor;

    // 图表
    QChart *chart;
    QList<QSplineSeries*> seriesList;
    QValueAxis *axisX;
    QValueAxis *axisY;

    // 串口
    QSerialPort *serialPort;

    // 接收数据变量
    QByteArray RecvBuff;
    QByteArray TempRe;

    // 发送数据变量
    QString     SendTextStr;
    QByteArray  SendTextByte;
    // 记录收集的数据量 Rx
    quint32     dataRxNumber = 0 ;
    // 串口状态
    quint8      Serial_State = 0 ;

    int currentX = 0; // 类成员变量，用于追踪当前X轴的位置
    // 更新Y轴的变量
    double maxYValue = 5.0;
    double minYValue = 0.0;

private:
    // 主题box
    void populateThemeBox();
};

#endif // WIDGET_H
