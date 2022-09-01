#include "widget.h"
#include "ui_widget.h"

#include <QApplication>
#include <QProcess>
#include <QString>
int main(int argc, char *argv[])
{   
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("PeaceToolkit ——安卓工具“和”");   //改主窗口名字
    w.setWindowFlags(w.windowFlags() &~ Qt::WindowMaximizeButtonHint);  //禁止最大化
    w.show();
    return a.exec();
}
