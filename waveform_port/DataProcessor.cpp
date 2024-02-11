// DataProcessor.cpp
#include "DataProcessor.h"

DataProcessor::DataProcessor(QObject *parent) : QObject(parent) {}

void DataProcessor::processData(const QByteArray &serialBuffer, const int size) {
    QVector<double> processedData;
    int numValues = 0;

    lines = serialBuffer.split('\n');
    for (int i = 0; i < lines.size() - 1; ++i) { // 最后一行可能是空的或者不完整
        QList<QByteArray> values = lines[i].split(',');
        numValues = qMin(values.size(), size);
        for (int j = 0; j < numValues; ++j) { // 遍历所有值
            bool ok;
            double value = values[j].toDouble(&ok);
            if (ok) {
                processedData.push_back(value);
                }
            }
        }

    LeftBuff = lines.last(); // 处理剩下的数据发送回去
    emit updateChart(processedData, LeftBuff, numValues); // 发射信号，携带处理好的数据
}


