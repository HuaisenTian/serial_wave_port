#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H
#include <QObject>
#include <QByteArray>

class DataProcessor : public QObject {
    Q_OBJECT

public:
    explicit DataProcessor(QObject *parent = nullptr);

signals:
    void updateChart(const QVector<double> &data, const QByteArray &leftbuff, const int numValues); // 图表中显示的数据类型

public slots:
    void processData(const QByteArray &serialBuffer, const int size); // 这个槽用于处理从串口接收的数据

private:
    QList<QByteArray> lines;
    QByteArray LeftBuff;
};

#endif // DATAPROCESSOR_H
