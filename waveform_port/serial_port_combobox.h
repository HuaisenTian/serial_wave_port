#ifndef SERIAL_PORT_COMBOBOX_H
#define SERIAL_PORT_COMBOBOX_H

#include <QComboBox>
#include <QMouseEvent>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <DataProcessor.h>

class serial_port_combobox : public QComboBox
{
    Q_OBJECT
public:
    explicit serial_port_combobox(QWidget *parent = nullptr);

    // 鼠标点击事件
    void mousePressEvent(QMouseEvent *event);

signals:

public slots:

private:
    void scanActivePort();
};

#endif // SERIAL_PORT_COMBOBOX_H

