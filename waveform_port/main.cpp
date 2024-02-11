#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // 适应不同屏幕
    if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
           QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    // 注册QVector<double>类型
    qRegisterMetaType<QVector<double>>("QVector<double>");
    Widget w;
    w.show();

    return a.exec();
}
