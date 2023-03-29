#include "widget.h"
#include "ui_widget.h"

#include <QString>
#include <QMessageBox>
#include <QProcess>
#include <QTimer>
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QFile>
#include <QDesktopServices>
#include <QFileDialog>
#include <QRegularExpression>
#include <QTime>
#include <string>
//网络相关头文件
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
//JSON解析相关头文件
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{      
    ui->setupUi(this);

    DeviceSerialNumber = "测试";

    QTimer* timer0 = new QTimer(this);
    timer0->start(1000);
    connect(timer0, &QTimer::timeout, this, [=]()
    {   
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb devices");
        process->waitForStarted();
        process->waitForFinished();
        QString output = QString::fromLocal8Bit(process->readAllStandardOutput());
        if(output != DeviceSerialNumber)
        {
            if(output == "List of devices attached\r\n\r\n")    //检验是否有设备连接
            {
                ui->brand->setText("品牌：获取中······");
                ui->model->setText("型号：获取中······");
                ui->codename->setText("设备代号：获取中······");
                ui->AndroidVersion->setText("Android系统版本：获取中······");
                ui->AndoidSDKVersion->setText("Android SDK版本：获取中······");
                ui->CPUArchitecture->setText("CPU架构：获取中······");
                ui->resolution->setText("分辨率：获取中······");
                ui->DPI->setText("显示密度：获取中······");
                ui->batteryinfo->setText("电池基本信息：获取中······");
                ui->cpuplatforminfo->setText("CPU平台：获取中······");
                ui->buildversion->setText("编译版本：获取中······");
                ui->modelversion->setText("机型版本号：获取中······");
            }
            else
            {
                //获取品牌
                process = new QProcess(this);
                process->start("cmd", QStringList() << "/c" << "adb shell getprop ro.product.brand");
                process->waitForStarted();
                process->waitForFinished();
                QString brand = QString::fromLocal8Bit(process->readAllStandardOutput());
                ui->brand->setText("品牌："+brand);

                //获取型号
                process = new QProcess(this);
                process->start("cmd", QStringList() << "/c" << "adb shell getprop ro.product.model");
                process->waitForStarted();
                process->waitForFinished();
                QString model = QString::fromLocal8Bit(process->readAllStandardOutput());
                ui->model->setText("型号："+model);

                //获取设备代号
                process = new QProcess(this);
                process->start("cmd", QStringList() << "/c" << "adb shell getprop ro.product.board");
                process->waitForStarted();
                process->waitForFinished();
                QString codename = QString::fromLocal8Bit(process->readAllStandardOutput());
                ui->codename->setText("设备代号："+codename);

                //获取Android版本信息
                process = new QProcess(this);
                process->start("cmd", QStringList() << "/c" << "adb shell getprop ro.build.version.release");
                process->waitForStarted();
                process->waitForFinished();
                QString AndroidVersion = QString::fromLocal8Bit(process->readAllStandardOutput());
                ui->AndroidVersion->setText("Android系统版本："+AndroidVersion);
                AndroidVersionNumber = AndroidVersion;

                //获取Android SDK版本信息
                process = new QProcess(this);
                process->start("cmd", QStringList() << "/c" << "adb shell getprop ro.build.version.sdk");
                process->waitForStarted();
                process->waitForFinished();
                QString AndroidSDKVersion = QString::fromLocal8Bit(process->readAllStandardOutput());
                ui->AndoidSDKVersion->setText("Android SDK版本："+AndroidSDKVersion);

                //获取CPU架构信息
                process = new QProcess(this);
                process->start("cmd", QStringList() << "/c" << "adb shell getprop ro.product.cpu.abi");
                process->waitForStarted();
                process->waitForFinished();
                QString CPUArchitecture = QString::fromLocal8Bit(process->readAllStandardOutput());
                ui->CPUArchitecture->setText("CPU架构："+CPUArchitecture);

                //获取屏幕分辨率信息
                process = new QProcess(this);
                process->start("cmd", QStringList() << "/c" << "adb shell wm size");
                process->waitForStarted();
                process->waitForFinished();
                QString resolution = QString::fromLocal8Bit(process->readAllStandardOutput());
                resolution.replace(QString("Physical size: "), QString(""));    //去除ADB命令输出结果中的"Physical size: "
                ui->resolution->setText("分辨率："+resolution);

                //获取DPI信息
                process = new QProcess(this);
                process->start("cmd", QStringList() << "/c" << "adb shell wm density");
                process->waitForStarted();
                process->waitForFinished();
                QString dpi0 = QString::fromLocal8Bit(process->readAllStandardOutput());
                dpi0.replace(QString("Physical density: "), QString(""));    //去除ADB命令输出结果中的"Physical size: "
                QString dpi;
                if(dpi0.contains("Override density:"))
                {
                    QStringList list17 = dpi0.split("Override density:");
                    dpi = list17[1];
                }
                else
                {
                    dpi = dpi0;
                }
                ui->DPI->setText("显示密度："+dpi);
                //将DPI换算成dp，原理：dp*ppi/160 = px，其中在屏幕显示中ppi=dpi
                float dpi_float = dpi.toFloat();
                resolution.replace(QString("Physical size: "), QString(""));    //去除ADB命令输出结果中的"Physical size: "
                QStringList list16 = resolution.split("x");
                QString px = list16[0];
                px.replace("x", "");
                float px_float = px.toFloat();
                float dp_float = px_float*160/dpi_float;
                int dp_integer_float = (int)(dp_float+0.5f);      //对换算得到的dpi数据进行取整处理
                QString dp = QString::number(dp_integer_float);
                ui->DPIanddp->setText("显示密度（DPI）：" + dpi + "\n最小宽度（dp）：" + dp);

                //获取电池信息
                process = new QProcess(this);
                process->start("cmd", QStringList() << "/c" << "adb shell dumpsys battery");
                process->waitForStarted();
                process->waitForFinished();
                QString batteryinfo = QString::fromLocal8Bit(process->readAllStandardOutput());
                //获取电量百分比信息
                QStringList list0 = batteryinfo.split("level:");
                QString betteryinfo0 = list0[1];   //初步对电池信息按需求（电量百分比、电压、温度）进行切割
                QStringList list1 = betteryinfo0.split("scale");
                QString level = list1[0];
                level.replace(QString(" "), QString(""));   //电量百分比数字
                level.replace(QString("\n"), QString(""));      //去掉ADB调试输出电量百分数数据后的换行
                level.append("%   ");      //对电量百分比信息追加百分号"%"单位并增加空格扩大参数间距
                //获取电压信息
                QString batteryinfo1 = list1[1];
                QStringList list2 = batteryinfo1.split("voltage:");
                QString batteryinfo2 = list2[1];
                QStringList list3 = batteryinfo2.split("temperature:");
                QString voltage0 = list3[0];
                voltage0.replace(QString(" "), QString(""));     //此时得到的电压单位为毫伏，要进一步进行转换
                float float_voltage = voltage0.toFloat();   //把电压数据字符串向浮点型数字转换
                float v = float_voltage/1000;    //单位转换
                QString voltage = QString::number(v);   //把电压数据往字符串转换
                voltage.append("V   ");    //追加伏特"V"单位并增加空格扩大参数间距
                //获取温度信息
                QString batteryinfo3 = list3[1];
                QStringList list4 = batteryinfo3.split("technology");
                QString temperature0 = list4[0];
                temperature0.replace(QString(" "), QString(""));
                float float_temperature = temperature0.toFloat();
                float t = float_temperature/10;     //ADB命令输出的电池温度是将实际温度乘以10处理此处进行还原
                QString temperature = QString::number(t);
                temperature.append("℃");    //追加单位摄氏度"℃"
                ui->batteryinfo->setText("电池基本信息：" + level + voltage + temperature);

                //获取CPU信息
                process = new QProcess(this);
                process->start("cmd", QStringList() << "/c" << "adb shell cat /proc/cpuinfo");
                process->waitForStarted();
                process->waitForFinished();
                QString a = QString::fromLocal8Bit(process->readAllStandardOutput());
                if(a.contains("Hardware",Qt::CaseInsensitive))
                {
                    QStringList list10 = a.split("Hardware");      //QString字符串分割函数
                    QString b = list10[1];    //截取ADB调试输出结果的后半部分(CPU平台信息）
                    QString cpuplatforminfo = b.simplified();     //去除ADB调试输出Hardware后面的空格
                    cpuplatforminfo.replace(QString(":"),QString(""));      //去除ADB调试输出结果Hardware后的冒号":"
                    ui->cpuplatforminfo->setText("CPU平台："+cpuplatforminfo);
                }
                else
                {
                    QString cpuplatforminfo1 = ("N/A");
                    ui->cpuplatforminfo->setText("CPU平台："+cpuplatforminfo1);
                }

                //获取编译版本信息
                process = new QProcess(this);
                process->start("cmd", QStringList() << "/c" << "adb shell getprop ro.bootimage.build.version.incremental");
                process->waitForStarted();
                process->waitForFinished();
                QString buildversion = QString::fromLocal8Bit(process->readAllStandardOutput());
                ui->buildversion->setText("编译版本："+buildversion);

                //获取机型版本号
                process = new QProcess(this);
                process->start("cmd", QStringList() << "/c" << "adb shell getprop ro.product.name");
                process->waitForStarted();
                process->waitForFinished();
                QString modelversion = QString::fromLocal8Bit(process->readAllStandardOutput());
                ui->modelversion->setText("机型版本号："+modelversion);

                DeviceSerialNumber = output;
            }
        }
        else
        {
            DeviceSerialNumber = output;
        }
    });

    QTimer* timer1 = new QTimer(this);
    //按钮点击事件
    connect(ui->autocheck, &QPushButton::clicked, this, [=]()
    {
        //判断按钮是否已按下
        if(timer1->isActive())
        {
            timer1->stop();      //关闭定时器
            ui->autocheck->setText("开启自动获取");
        }
        else
        {
            ui->autocheck->setText("关闭自动获取");
            timer1->start(1000);     //启动定时器
        }
    });
    //定时器执行内容
    connect(timer1, &QTimer::timeout, this, [=]()
    {
        //获取当前软件包名和活动
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell dumpsys window | findstr mCurrentFocus");
        process->waitForStarted();
        process->waitForFinished();
        QString na = QString::fromLocal8Bit(process->readAllStandardOutput());
        if(na.contains("mCurrentFocus=null") || na.contains("Dialog for signing in.") || na.isNull())     //判断用户是否打开应用程序或者程序处于登录界面
        {
            QString state("获取中······");
            ui->packagename->setText(state);
            ui->activity->setText(state);
        }
        else
        {
            QStringList list13 = na.split("/");
            QString activity = list13[1];
            activity.replace(QString("}"), QString(""));
            ui->activity->setText(activity);       //获取当前应用活动
            QString n0 = list13[0];
            QStringList list14 = n0.split("Window{");
            QString n1 = list14[1];
            n1.replace(QString(" "), QString(""));
            while (n1.left(3) != "com")     //删除字符串处理至最后行首的无关字符
            {
                n1.remove(0,1);
            }
            QString packagename = n1;
            ui->packagename->setText(packagename);
            if(ui->autofill->isChecked())
            {
                if(ui->inputpackage->text() != packagename)
                {
                    ui->inputpackage->setText(packagename);
                }
            }
        }
    });

    //设置手动输入软件包名输入框的占位字符串
    ui->inputpackage->setPlaceholderText("输入软件包名：");

    //检查用于存放APK文件的文件夹apks是否存在，若不存在则创建
    if(!QDir(".\\apks").exists())
    {
        QDir().mkdir(".\\apks");
    }

    //设置横向像素和纵向像素输入框的占位字符
    ui->landscapepixels->setPlaceholderText("请输入横向像素：");
    ui->portraitpixels->setPlaceholderText("请输入纵向像素：");

    //锁定显示密度修改数据输入框
    ui->inputvalue->setPlaceholderText("请选择单位！");
    ui->inputvalue->setReadOnly(true);

    //软件管理页面的全选功能
    connect(ui->selectall, &QCheckBox::clicked, this, [=] ()
    {
        if(ui->selectall->isChecked())
        {
            ui->playstore->setChecked(true);
            ui->gboard->setChecked(true);
            ui->podcast->setChecked(true);
            ui->gmail->setChecked(true);
            ui->googleservice->setChecked(true);
            ui->googledrive->setChecked(true);
            ui->googlemovie->setChecked(true);
            ui->googlesearch->setChecked(true);
            ui->googleserviceframework->setChecked(true);
            ui->googlephotos->setChecked(true);
            ui->googleone->setChecked(true);
            ui->gmap->setChecked(true);
            ui->youtube->setChecked(true);
            ui->googlepay->setChecked(true);
            ui->chrome->setChecked(true);
            ui->googleduo->setChecked(true);
        }
        else
        {
            ui->playstore->setChecked(false);
            ui->gboard->setChecked(false);
            ui->podcast->setChecked(false);
            ui->gmail->setChecked(false);
            ui->googleservice->setChecked(false);
            ui->googledrive->setChecked(false);
            ui->googlemovie->setChecked(false);
            ui->googlesearch->setChecked(false);
            ui->googleserviceframework->setChecked(false);
            ui->googlephotos->setChecked(false);
            ui->googleone->setChecked(false);
            ui->gmap->setChecked(false);
            ui->youtube->setChecked(false);
            ui->googlepay->setChecked(false);
            ui->chrome->setChecked(false);
            ui->googleduo->setChecked(false);
        }
    });

    //状态栏图标隐藏的全选功能
    connect(ui->all, &QCheckBox::clicked, [=] ()
    {
        if(ui->all->isChecked())
        {
            ui->location->setChecked(true);
            ui->earphone->setChecked(true);
            ui->volte->setChecked(true);
            ui->battery->setChecked(true);
            ui->time->setChecked(true);
            ui->signal->setChecked(true);
            ui->airplane->setChecked(true);
            ui->hotspot->setChecked(true);
            ui->cast->setChecked(true);
            ui->wifi->setChecked(true);
            ui->speakerphone->setChecked(true);
            ui->nfc->setChecked(true);
            ui->bluetooth->setChecked(true);
            ui->alarmclock->setChecked(true);
            ui->zen->setChecked(true);
            ui->powersavingmode->setChecked(true);
            ui->mute_2->setChecked(true);
            ui->volume->setChecked(true);
            ui->eyesprotect->setChecked(true);
        }
        else
        {
            ui->location->setChecked(false);
            ui->earphone->setChecked(false);
            ui->volte->setChecked(false);
            ui->battery->setChecked(false);
            ui->time->setChecked(false);
            ui->signal->setChecked(false);
            ui->airplane->setChecked(false);
            ui->hotspot->setChecked(false);
            ui->cast->setChecked(false);
            ui->wifi->setChecked(false);
            ui->speakerphone->setChecked(false);
            ui->nfc->setChecked(false);
            ui->bluetooth->setChecked(false);
            ui->alarmclock->setChecked(false);
            ui->zen->setChecked(false);
            ui->powersavingmode->setChecked(false);
            ui->mute_2->setChecked(false);
            ui->volume->setChecked(false);
            ui->eyesprotect->setChecked(false);
        }
    });

    //设置状态栏沉浸排除软件包名输入框的占位字符串
    ui->ExcludeApps->setPlaceholderText("排除软件");

    //电池管理页面相关输入框占位字符串设置
    ui->temperatedit->setPlaceholderText("温度(℃)");
    ui->leveledit->setPlaceholderText("电量(%)");

    //设置电量统计的按钮不可用
    ui->StartBatteryCounting->setEnabled(false);

    //电量统计功能
    QTimer* timer2 = new QTimer(this);
    //按钮点击事件
    connect(ui->StartBatteryCounting, &QPushButton::clicked, this, [=]()
    {
        //判断按钮是否已按下
        if(timer2->isActive())
        {
            timer2->stop();      //关闭定时器
            ui->StartBatteryCounting->setText("开始");
        }
        else
        {
            time.setHMS(0,0,0,0);
            ui->StartBatteryCounting->setText("停止");
            //清空计时数据
            ui->Time->clear();
            ui->Level->clear();
            ui->Time->setText(time.toString("hh:mm:ss"));
            //获取电池信息
            process = new QProcess(this);
            process->start("cmd", QStringList() << "/c" << "adb shell dumpsys battery");
            process->waitForStarted();
            process->waitForFinished();
            QString batteryinfo = QString::fromLocal8Bit(process->readAllStandardOutput());
            //获取电量百分比信息
            QStringList list0 = batteryinfo.split("level:");
            QString betteryinfo0 = list0[1];   //初步对电池信息按需求（电量百分比、电压、温度）进行切割
            QStringList list1 = betteryinfo0.split("scale");
            QString level = list1[0];
            level.replace(QString(" "), QString(""));   //电量百分比数字
            level.replace(QString("\n"), QString(""));      //去掉ADB调试输出电量百分数数据后的换行
            level.replace(QString("\r"),QString(""));   //去掉数据的回车
            ui->Level->setText(level);
            timer2->start(CountingTime);     //启动定时器
        }
    });   
    //定时器执行内容
    connect(timer2, &QTimer::timeout, this, [=]()
    {
        //输出电量
        time = time.addSecs(CountingTime/1000);
        ui->Time->append(time.toString("hh:mm:ss"));
        //获取电池信息
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell dumpsys battery");
        process->waitForStarted();
        process->waitForFinished();
        QString batteryinfo = QString::fromLocal8Bit(process->readAllStandardOutput());
        //获取电量百分比信息
        QStringList list0 = batteryinfo.split("level:");
        QString betteryinfo0 = list0[1];   //初步对电池信息按需求（电量百分比、电压、温度）进行切割
        QStringList list1 = betteryinfo0.split("scale");
        QString level = list1[0];
        level.replace(QString(" "), QString(""));   //电量百分比数字
        level.replace(QString("\n"), QString(""));      //去掉ADB调试输出电量百分数数据后的换行
        level.replace(QString("\r"),QString(""));   //去掉数据的回车
        ui->Level->append(level);
    });

    QTimer* timer3 = new QTimer(this);
    timer3->start(1000);
    connect(timer3, &QTimer::timeout, this, [=]()
    {
        //获取电池信息
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell dumpsys battery");
        process->waitForStarted();
        process->waitForFinished();
        QString batteryinfo = QString::fromLocal8Bit(process->readAllStandardOutput());
        ui->BatteryInfoBrowser->setText(batteryinfo);
    });

    //初始化字体缩放滑动条
    ui->FontScaling->setRange(0,400);
    //禁止鼠标和键盘单点操作（否则写代码太麻烦）
    ui->FontScaling->setSingleStep(0);
    ui->FontScaling->setPageStep(0);
    //设置刻度
    ui->FontScaling->setTickInterval(100);
    ui->FontScaling->setTickPosition(QSlider::TicksBelow);

    //获取当前字体缩放倍数
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings get system font_scale");
    process->waitForStarted();
    process->waitForFinished();
    CurrentFontZoomMultipliter = QString::fromLocal8Bit(process->readAllStandardOutput());
    CurrentFontZoomMultipliter.replace(QString("\r"), QString(""));
    CurrentFontZoomMultipliter.replace(QString("\n"), QString(""));
    ui->FontScalingShower->setText("设定倍数：" + CurrentFontZoomMultipliter + "x");
    ui->FontScalingShower->append("当前倍数：" + CurrentFontZoomMultipliter + "x");
    ui->FontScaling->setValue(CurrentFontZoomMultipliter.toFloat()*100);    //给滑块赋值

    //初始化动画速度相关进度条
    ui->WindowAnimation->setRange(0,400);
    ui->TransitionAnimations->setRange(0,400);
    ui->AnimatorDuration->setRange(0,400);

    //禁止鼠标和键盘单点操作（原因同上）
    ui->WindowAnimation->setSingleStep(0);
    ui->WindowAnimation->setPageStep(0);
    ui->TransitionAnimations->setSingleStep(0);
    ui->TransitionAnimations->setPageStep(0);
    ui->AnimatorDuration->setSingleStep(0);
    ui->AnimatorDuration->setPageStep(0);

    //分别获取当前窗口、过渡和程序动画速度
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings get global window_animation_scale");
    process->waitForStarted();
    process->waitForFinished();
    CurrentWindowAnimation = QString::fromLocal8Bit(process->readAllStandardOutput());
    CurrentWindowAnimation.remove(QString("\r") + QString("\n"));
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings get global transition_animation_scale");
    process->waitForStarted();
    process->waitForFinished();
    CurrentTransitionAnimations = QString::fromLocal8Bit(process->readAllStandardOutput());
    CurrentTransitionAnimations.remove(QString("\r") + QString("\n"));
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings get global animator_duration_scale");
    process->waitForStarted();
    process->waitForFinished();
    CurrentAnimatorDuration = QString::fromLocal8Bit(process->readAllStandardOutput());
    CurrentAnimatorDuration.remove(QString("\r") + QString("\n"));

    //分别给动画速度相关控制滑块赋予初始值
    ui->WindowAnimation->setValue(CurrentWindowAnimation.toFloat()*100);
    ui->TransitionAnimations->setValue(CurrentTransitionAnimations.toFloat()*100);
    ui->AnimatorDuration->setValue(CurrentAnimatorDuration.toFloat()*100);

    //将动画速度相关参数的初始值打印到文本框上
    ui->AnimationSpeed->setText("窗口速度：" + CurrentWindowAnimation + "x");
    ui->AnimationSpeed->append("过渡速度：" + CurrentTransitionAnimations + "x");
    ui->AnimationSpeed->append("程序速度：" + CurrentAnimatorDuration + "x");

    //设置动画速度相关控制滑块的刻度线
    ui->WindowAnimation->setTickInterval(100);
    ui->WindowAnimation->setTickPosition(QSlider::TicksBelow);
    ui->TransitionAnimations->setTickInterval(100);
    ui->TransitionAnimations->setTickPosition(QSlider::TicksBelow);
    ui->AnimatorDuration->setTickInterval(100);
    ui->AnimatorDuration->setTickPosition(QSlider::TicksBelow);

    QTimer* timer4 = new QTimer(this);
    timer4->start(1000);
    connect(timer4, &QTimer::timeout, this, [=]()
    {
        //获取当前锁屏时间
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell settings get system screen_off_timeout");
        process->waitForStarted();
        process->waitForFinished();
        ScreenOffTime = QString::fromLocal8Bit(process->readAllStandardOutput());
        if(ScreenOffTime == "2147483647")
        {
            ui->CurrentScreenOffTime->setText("当前设备永不锁屏");
        }
        else
        {
            float_CurrentScreenOffTime = ScreenOffTime.toFloat();
            ScreenOffTime = QString::number(float_CurrentScreenOffTime/1000);
            ui->CurrentScreenOffTime->setText("当前锁屏时间：" + ScreenOffTime + "s");
        }
    });

    //设置自动锁屏时间输入框的占位字符串
    ui->ScreenOff->setPlaceholderText("请选择单位!");

    //设置自动锁屏时间输入框为不可输入状态
    ui->ScreenOff->setEnabled(false);

    //设置自动锁屏时间修改按钮为不可用
    ui->ChangeScreenOffTime->setEnabled(false);

    //设置ADB命令输入框占位字符串
    ui->InputADBCommand->setPlaceholderText("请输入命令，回车后执行！");

    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb devices");
    process->waitForStarted();
    process->waitForFinished();
    QString ADBDevices = QString::fromLocal8Bit(process->readAllStandardOutput());
    ui->ADBOutputBrowser->setText(ADBDevices);

    //检查用于存放镜像文件的文件夹imgs是否存在，若不存在则创建
    if(!QDir(".\\imgs").exists())
    {
        QDir().mkdir(".\\imgs");
    }

    //设置分区操作按钮均为不可用状态
    ui->ErasePartition->setEnabled(false);
    ui->FormatPartition->setEnabled(false);
    ui->FlashPartition->setEnabled(false);

    //获取ADB版本信息
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb version");
    process->waitForStarted();
    process->waitForFinished();
    QString GetADBVersion = QString::fromLocal8Bit(process->readAllStandardOutput());
    QStringList list = GetADBVersion.split("Version");
    QString ADBVersion = list[0];
    ADBVersion.remove("\r\n");
    ADBVersion.replace(QString("Android Debug Bridge version "), QString("ADB版本："));
    ui->SoftwareVersionInfo->append(ADBVersion);

    //检查用于存放JSON文件的文件夹JSON是否存在，若不存在则创建
    if(!QDir(".\\JSON").exists())
    {
        QDir().mkdir(".\\JSON");
    }

    //输出当前软件版本
    ui->SoftwareVersionInfo->append("软件版本：" + CurrentVersion);

    //网络验证授权相关槽函数绑定，暂时用不到
    /*
    connect(&qnam, &QNetworkAccessManager::authenticationRequired,
            this, &Widget::slotAuthenticationRequired);
    */
    connect(ui->CheckUpdate, &QAbstractButton::clicked, this, &Widget::downloadFile);
}

Widget::~Widget()
{
    delete ui;
}

//比较版本大小函数
int Widget::VersionCompare(const QString &Version1, const QString &Version2)
{
    QStringList VersionList1 = Version1.split(".");
    QStringList VersionList2 = Version2.split(".");
    int ListCount1 = VersionList1.count();
    int ListCount2 = VersionList2.count();
    int nCount = ListCount1 > ListCount2 ? ListCount2 : ListCount1;
    for(int i = 0; i < nCount; ++i)
    {
        //判断转换是否成功
        bool OK1 = false;
        bool OK2 = false;
        int number1 = VersionList1[i].toInt(&OK1);
        int number2 = VersionList2[i].toInt(&OK2);
        if(!OK1 || !OK2)    //转换失败返回-2
        {
            return -2;
        }
        if(number1 == number2)
        {
            continue;
        }
        if(number1 > number2)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
    return 1;
}

void Widget::startRequest(const QUrl &requestedUrl)
{
    url = requestedUrl;
    httpRequestAborted = false;
    //! [qnam-download]
        reply.reset(qnam.get(QNetworkRequest(url)));
        //! [qnam-download]
        //! [connecting-reply-to-slots]
        connect(reply.get(), &QNetworkReply::finished, this, &Widget::httpFinished);
        //! [networkreply-readyread-1]
        connect(reply.get(), &QIODevice::readyRead, this, &Widget::httpReadyRead);
        //! [networkreply-readyread-1]
#if QT_CONFIG(ssl)
    //! [sslerrors-1]
    connect(reply.get(), &QNetworkReply::sslErrors, this, &Widget::sslErrors);
    //! [sslerrors-1]
#endif
}

void Widget::downloadFile()
{
    //JSON文件存放网址
    QUrl newUrl("https://github.com/CN-Scars/JSON-Repository/releases/download/PeaceToolkit_JSON/PeaceToolkit_update.json");
    if(!newUrl.isValid())      //检测JSON文件地址是否失效
    {
        QMessageBox::information(this, tr("错误！"),
                                 tr("软件更新信息获取地址失效，请联系开发者！"));
        return;
    }
    QString fileName = "PeaceToolkit_update.json";
    QString downloadDirectory = "./JSON/";     //JSON文件下载路径
    fileName.prepend(downloadDirectory + '/');
    if(QFile::exists(fileName))     //若已存在JSON文件就删除
    {
        QFile::remove(fileName);
    }
    file = openFileForWrite(fileName);
    if (!file)
        return;
    //安排请求
    startRequest(newUrl);   //将http网址传递到startRequest槽函数中
}

std::unique_ptr<QFile> Widget::openFileForWrite(const QString &fileName)
{
    std::unique_ptr<QFile> file = std::make_unique<QFile>(fileName);
    if(!file->open(QIODevice::WriteOnly))
    {
        QMessageBox::information(this, tr("错误"),
                                 tr("无法保存该JSON文件 %1: %2.")
                                 .arg(QDir::toNativeSeparators(fileName),
                                      file->errorString()));
        return nullptr;
    }
    return file;
}

void Widget::httpFinished()
{
    QFileInfo fi;
    if(file)
    {
        fi.setFile(file->fileName());
        file->close();
        file.reset();
    }
    //! [networkreply-error-handling-1]
       QNetworkReply::NetworkError error = reply->error();
       const QString &errorString = reply->errorString();
       //! [networkreply-error-handling-1]
       reply.reset();
       //! [networkreply-error-handling-2]
       if(error != QNetworkReply::NoError)
       {
           QFile::remove(fi.absoluteFilePath());
           if(!httpRequestAborted)
           {
               QMessageBox::information(this, tr("错误"),
                                        tr("JSON文件下载失败，错误信息：").arg(errorString));
           }
           return;
       }
       //! [networkreply-error-handling-2]
       ParseJSON(true);     //给解析JSON槽函数传递bool变量信号
}

//! [networkreply-readyread-2]
void Widget::httpReadyRead()
{
    //QNetworkReply信号
    if(file)
        file->write(reply->readAll());
}
//! [networkreply-readyread-2]

/*  网络验证授权，暂时用不到
//! [qnam-auth-required-2]
void Widget::slotAuthenticationRequired(QNetworkReply *, QAuthenticator *authenticator)
{

}
//! [qnam-auth-required-2]
*/

#if QT_CONFIG(ssl)
//! [sslerrors-2]
void Widget::sslErrors(const QList<QSslError> &errors)
{
    QString errorString;
    for (const QSslError &error : errors) {
        if (!errorString.isEmpty())
            errorString += '\n';
        errorString += error.errorString();
    }

    if (QMessageBox::warning(this, tr("SSL Errors"),
                             tr("发生了一个或多个 SSL 错误信息:\n%1").arg(errorString),
                             QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore) {
        reply->ignoreSslErrors();
    }
}
//! [sslerrors-2]
#endif

void Widget::ParseJSON(bool a)
{
    if(a==true)
    {
        //解析JSON
        QFile loadFile(".\\JSON\\PeaceToolkit_update.json");     //读取JSON文件
        if(!loadFile.open(QIODevice::ReadOnly))     //判断是否成功打开JSON文件
        {
            qDebug() << "could't open projects json";
            return;
        }
        QByteArray allData = loadFile.readAll();    //把JSON文件数据转化为QByteArray类型
        loadFile.close();   //关闭文件
        QJsonParseError json_error;     //JSON解析错误报告信息类
        QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));   //JSON文档类
        if(json_error.error != QJsonParseError::NoError)    //判断JSON解析是否出错
        {
            qDebug() << "json error!";
            return;
        }
        QJsonObject rootObj = jsonDoc.object();     //创建用于封装JSON对象的对象
        QStringList keys = rootObj.keys();      //用QStringList类存储JSON数据的键
        for(int i = 0; i < keys.size(); i++)
        {
            qDebug() << "key" << i << " is:" << keys.at(i);
        }
        if(rootObj.contains("PeaceToolkit") && rootObj["PeaceToolkit"].isObject())    //判断目标键是否存在且为键
        {
            QJsonObject subObj = rootObj["PeaceToolkit"].toObject();
            if (subObj.contains("LatestVersion") && subObj["LatestVersion"].isString())     //获取最新版本
            {
                LatestVersion = subObj["LatestVersion"].toString();
            }
            if (subObj.contains("Url") && subObj["Url"].isString())
            {
                Url = subObj["Url"].toString();
            }
            if (subObj.contains("UpdateTime") && subObj["UpdateTime"].isString())
            {
                UpdateTime = subObj["UpdateTime"].toString();
            }
            if (subObj.contains("ReleaseNote") && subObj["ReleaseNote"].isString())
            {
                ReleaseNote = subObj["ReleaseNote"].toString();
            }
        }
    }
    else
    {
        return;
    }
    //比较版本号
    CurrentVersion.remove("V");
    LatestVersion.remove("V");
    int result = VersionCompare(CurrentVersion, LatestVersion);
    QString Result;
    switch(result)
    {
        case -2:
            Result = "检测版本更新失败！";
        break;
        case 0:
            Result = "检测到新版本！是否前往更新？";
        break;
        case 1:
            Result = "当前已是最新版本！";
        break;
        case -1:
            Result = "当前已是最新版本！";
        break;
        default:
        break;
    }
    if(result == -2)
    {
        QMessageBox error;
        error.setWindowTitle("错误！");
        error.setText(Result);
        error.exec();
    }
    else if(result == 1 || result == -1)
    {
        QMessageBox notice;
        notice.setWindowTitle("注意！");
        notice.setText(Result + "\n最新版本号：V" + LatestVersion + "\n更新时间：" + UpdateTime + "\n更新日志：\n" + ReleaseNote);
        notice.exec();
    }
    else
    {
        QString UpdateNotice;
        UpdateNotice = Result + "\n注意：该软件不支持覆盖更新，请下载新版本软件安装包后手动卸载旧版本软件再进行安装，敬请谅解！\n最新版本号：V" + LatestVersion + "\n更新时间：" + UpdateTime + "\n更新日志：\n" + ReleaseNote;
        QMessageBox confirm;
        confirm.setWindowTitle("注意！");
        confirm.setText(UpdateNotice);
        confirm.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
        if(confirm.exec() == QMessageBox::No)
        {
            return;
        }
        else
        {
            QDesktopServices::openUrl(QUrl(Url.toLatin1()));
        }
    }
}

void Widget::on_Tabs_currentChanged(int index)
{
    int TabPageNumber = index;
    if(TabPageNumber == 3)    //系统界面
    {
        QMessageBox notice;
        notice.setWindowTitle("注意");
        notice.setText("部分功能Android10版本及以上可能无效，属正常现象！");
        notice.exec();
    }
}

//检查是否存在并创建提取APK存放文件夹
void CreateFolder(QString folderPath = QString(".\\apks"))
{
    //创建文件夹
    QDir dir(folderPath);

    if (!dir.exists())
    {
        dir.mkdir(folderPath);
    }
}

void Widget::on_reboot_clicked()
{
    //process = new QProcess();   //创建进程
        QStringList args;
            args.append("reboot");
    process->startDetached(".\\adb.exe",args);
}


void Widget::on_rebootrecovery_clicked()
{
        QStringList args;
            args.append("reboot");
            args.append("recovey");
    process->startDetached(".\\adb.exe",args);
}


void Widget::on_rebootfastboot_clicked()
{
        QStringList args;
            args.append("reboot");
            args.append("bootloader");
    process->startDetached(".\\adb.exe",args);
}


void Widget::on_poweroff_clicked()
{
        QStringList args;
            args.append("shell");
            args.append("reboot");
            args.append("-p");
    process->startDetached(".\\adb.exe",args);
}


void Widget::on_adbreboot9008_clicked()
{
        QStringList args;
            args.append("reboot");
            args.append("edl");
    process->startDetached(".\\adb.exe",args);
}


void Widget::on_fastbootreboot9008_clicked()
{
        QStringList args;
            args.append("oem");
            args.append("edl");
    process->startDetached(".\\fastboot.exe",args);
}


void Widget::on_back_clicked()
{
        QStringList args;
            args.append("shell");
            args.append("input");
            args.append("keyevent");
            args.append("4");
    process->startDetached(".\\adb.exe",args);
}


void Widget::on_home_clicked()
{
        QStringList args;
            args.append("shell");
            args.append("input");
            args.append("keyevent");
            args.append("3");
    process->startDetached(".\\adb.exe",args);
}


void Widget::on_menu_clicked()
{
        QStringList args;
            args.append("shell");
            args.append("input");
            args.append("keyevent");
            args.append("82");
    process->startDetached(".\\adb.exe",args);
}


void Widget::on_power_clicked()
{    
        QStringList args;
            args.append("shell");
            args.append("input");
            args.append("keyevent");
            args.append("26");
    process->startDetached(".\\adb.exe",args);
}


void Widget::on_screenshot_clicked()
{
        QStringList args;
            args.append("shell");
            args.append("screencap");
            args.append("-p");
            args.append("/sdcard/screenshot.png");
    process->startDetached(".\\adb.exe",args);
}


void Widget::on_tasks_clicked()
{
        QStringList args;
            args.append("shell");
            args.append("input");
            args.append("keyevent");
            args.append("187");
    process->startDetached(".\\adb.exe",args);
}


void Widget::on_volumeup_clicked()
{
        QStringList args;
            args.append("shell");
            args.append("input");
            args.append("keyevent");
            args.append("24");
    process->startDetached(".\\adb.exe",args);
}


void Widget::on_volumedown_clicked()
{
        QStringList args;
            args.append("shell");
            args.append("input");
            args.append("keyevent");
            args.append("25");
    process->startDetached(".\\adb.exe",args);
}


void Widget::on_mute_clicked()
{
        QStringList args;
            args.append("shell");
            args.append("input");
            args.append("keyevent");
            args.append("164");
    process->startDetached(".\\adb.exe",args);
}


void Widget::on_brightnessup_clicked()
{
        QStringList args;
            args.append("shell");
            args.append("input");
            args.append("keyevent");
            args.append("221");
    process->startDetached(".\\adb.exe",args);
}


void Widget::on_brightnessdown_clicked()
{
        QStringList args;
            args.append("shell");
            args.append("input");
            args.append("keyevent");
            args.append("220");
    process->startDetached(".\\adb.exe",args);
}


void Widget::on_voiceassistant_clicked()
{
        QStringList args;
            args.append("shell");
            args.append("input");
            args.append("keyevent");
            args.append("231");
    process->startDetached(".\\adb.exe",args);
}

void Widget::on_getnameandactivity_clicked()
{
    //获取当前软件包名和活动
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell dumpsys window | findstr mCurrentFocus");
    process->waitForStarted();
    process->waitForFinished();
    QString na = QString::fromLocal8Bit(process->readAllStandardOutput());
    QStringList list13 = na.split("/");
    QString activity = list13[1];
    activity.replace(QString("}"), QString(""));
    ui->activity->setText(activity);       //获取当前应用活动
    QString n0 = list13[0];
    QStringList list14 = n0.split("Window{");
    QString n1 = list14[1];
    n1.replace(QString(" "), QString(""));
    while (n1.left(3) != "com")     //删除字符串处理至最后行首的无关字符
    {
        n1.remove(0,1);
    }
    QString packagename = n1;
    ui->packagename->setText(packagename);
    if(ui->autofill->isChecked())
    {
        ui->inputpackage->setText(packagename);
    }
}

void Widget::on_disable_clicked()
{
    QString inputpackagename = ui->inputpackage->text();
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell pm disable-user" << inputpackagename);
    process->waitForStarted();
    process->waitForFinished();
}

void Widget::on_enable_clicked()
{
    QString inputpackagename = ui->inputpackage->text();
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell pm enable" << inputpackagename);
    process->waitForStarted();
    process->waitForFinished();
}

void Widget::on_extractAPK_clicked()
{
    if(!ui->inputpackage->text().isEmpty())
    {
        //获取要提取的APK路径
        QString inputpackagename = ui->inputpackage->text();
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm path" << inputpackagename);
        process->waitForStarted();
        process->waitForFinished();
        QString APKpath = QString::fromLocal8Bit(process->readAllStandardOutput());
        QStringList list15 = APKpath.split("package:");
        QString apkpath = list15[1];
        apkpath.replace(QString("\r"), QString(""));
        apkpath.replace(QString("\n"), QString(""));
        if(apkpath.contains("package:"))    //检验系统是否允许提取软件安装包
        {
            QMessageBox noticemessage;
            noticemessage.setWindowTitle("失败！");
            noticemessage.setText("系统禁止提取该软件安装包，请选择其它软件！");
            noticemessage.exec();
        }
        else
        {
            process = new QProcess(this);
            process->start("cmd", QStringList() << "/c" << "adb pull" << apkpath << "./apks/base.apk");
            process->waitForStarted();
            process->waitForFinished();
        }
        //给获取的安装包重命名，格式为“包名.apk”
        QFile apk(".\\apks\\base.apk");
        apk.rename(".\\apks\\" + inputpackagename +".apk");
    }
}

void Widget::on_openAPKfolder_clicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(".\\apks"));
}

void Widget::on_uninstall_clicked()
{
    QString inputpackagename = ui->inputpackage->text();
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb uninstall" << inputpackagename);
    process->waitForStarted();
    process->waitForFinished();
}

void Widget::on_uninstallkeepdata_clicked()
{
    QString inputpackagename = ui->inputpackage->text();
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell pm uninstall -k" << inputpackagename);
    process->waitForStarted();
    process->waitForFinished();
}

void Widget::on_cleardata_clicked()
{
    QString inputpackagename = ui->inputpackage->text();
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell pm clear" << inputpackagename);
    process->waitForStarted();
    process->waitForFinished();
}

void Widget::on_forcestop_clicked()
{
    QString inputpackagename = ui->inputpackage->text();
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell am force-stop" << inputpackagename);
    process->waitForStarted();
    process->waitForFinished();
}

void Widget::on_appinfo_clicked()
{
    if(!ui->inputpackage->text().isEmpty())
    {
        QString inputpackagename = ui->inputpackage->text();
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell dumpsys package" << inputpackagename);
        process->waitForStarted();
        process->waitForFinished();
        QString AppInfo = QString::fromLocal8Bit(process->readAllStandardOutput());
        QMessageBox appinfo;
        appinfo.setWindowTitle("软件包信息");
        appinfo.setText(AppInfo);
        appinfo.exec();
    }
}

void Widget::on_install_clicked()
{
    //选择后缀名为apk的文件
    QSettings path("./path.ini", QSettings::IniFormat);     //保存文件路径至运行目录
    QString lastpath = path.value("LastPath").toString();       //获取路径
    QString PathName = QFileDialog::getOpenFileName(this, "请选择apk安装包", lastpath, "*apk");
    if(!PathName.isEmpty())
    {
        path.value("LastPath", PathName);
    }
    /*
    //为路径字符串加上""，获取的路径不含有""，而ADB命令中路径必须含有""
    PathName.append("\"");
    PathName.prepend("\"");
    */
    //qDebug().noquote() << PathName;     //对含有转义符的字符串字符进行打印调试最好用qDebug.noquote，否则qDebug会原样输出转义符
    //安装apk
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb devices");
    process->waitForStarted();
    process->waitForFinished();
    QString serialnumber = QString::fromLocal8Bit(process->readAllStandardOutput());
    if(serialnumber.isNull())
    {
        QMessageBox notice;
        notice.setWindowTitle("错误！");
        notice.setText("设备未连接，请重试！");
        notice.exec();
    }
    else
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb install -r" << PathName);
        process->waitForStarted();
        process->waitForFinished();
        QString state = QString::fromLocal8Bit(process->readAllStandardOutput());
        if(state == "Performing Streamed Install\r\nSuccess\r\n")
        {
            QMessageBox notice;
            notice.setWindowTitle("成功！");
            notice.setText("应用安装已完成！");
            notice.exec();
        }
        else
        {
            QMessageBox notice;
            notice.setWindowTitle("失败！");
            notice.setText("安装失败，请重试！");
            notice.exec();
        }
    }
}

void Widget::on_sendfile_clicked()
{
    //选择文件
    QString PathName = QFileDialog::getOpenFileName(this, "请选择要发送的文件", "C:/");
    //发送文件
    QString Download("storage/emulated/0/Download");    //发送文件至Android设备的Download文件夹
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb push" << PathName << Download);
    process->waitForStarted();
    process->waitForFinished();
    QString state = QString::fromLocal8Bit(process->readAllStandardOutput());
    if(!state.contains("1 file pushed") && !state.isEmpty())
    {
        QMessageBox notice;
        notice.setWindowTitle("失败！");
        notice.setText("文件传输失败，请重试！");
        notice.exec();
    }
    else if(state.contains("1 file pushed") && !state.isEmpty())
    {
        QMessageBox notice;
        notice.setWindowTitle("成功！");
        notice.setText("文件已发送至Download文件夹！");
        notice.exec();
    }
}

void Widget::on_disableselected_clicked()
{
    //禁用Play商店
    if(ui->playstore->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm disable-user com.android.vending");
        process->waitForStarted();
        process->waitForFinished();
    }
    //禁用Gboard
    if(ui->gboard->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm disable-user com.google.android.inputmethod.latin");
        process->waitForStarted();
        process->waitForFinished();
    }
    //禁用谷歌播客
    if(ui->podcast->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm disable-user com.google.android.googlequicksearchbox");
        process->waitForStarted();
        process->waitForFinished();
    }
    //禁用GMail
    if(ui->gmail->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm disable-user com.google.android.gm");
        process->waitForStarted();
        process->waitForFinished();
    }
    //禁用Google服务
    if(ui->googleservice->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm disable-user com.google.android.ims");
        process->waitForStarted();
        process->waitForFinished();
    }
    //禁用Google Drive云盘
    if(ui->googledrive->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm disable-user com.google.android.apps.docs");
        process->waitForStarted();
        process->waitForFinished();
    }
    //禁用Google电影（TV）
    if(ui->googlemovie->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm disable-user com.google.android.videos");
        process->waitForStarted();
        process->waitForFinished();
    }
    //禁用Google搜索
    if(ui->googlesearch->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm disable-user com.google.android.googlequicksearchbox");
        process->waitForStarted();
        process->waitForFinished();
    }
    //禁用Google服务框架
    if(ui->googleserviceframework->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm disable-user com.google.android.gms");
        process->waitForStarted();
        process->waitForFinished();
    }
    //禁用Google相册
    if(ui->googlephotos->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm disable-user com.google.android.apps.photos");
        process->waitForStarted();
        process->waitForFinished();
    }
    //禁用Google One服务
    if(ui->googleone->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm disable-user com.google.android.apps.subscriptions.red");
        process->waitForStarted();
        process->waitForFinished();
    }
    //禁用GMap
    if(ui->gmap->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm disable-user com.google.android.apps.maps");
        process->waitForStarted();
        process->waitForFinished();
    }
    //禁用YouTube
    if(ui->youtube->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm disable-user com.google.android.youtube");
        process->waitForStarted();
        process->waitForFinished();
    }
    //禁用Google Pay
    if(ui->googlepay->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm disable-user com.google.android.apps.nbu.paisa.user");
        process->waitForStarted();
        process->waitForFinished();
    }
    //禁用Chrome
    if(ui->chrome->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm disable-user com.android.chrome");
        process->waitForStarted();
        process->waitForFinished();
    }
    //禁用Google Duo
    if(ui->googleduo->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm disable-user com.google.android.apps.tachyon");
        process->waitForStarted();
        process->waitForFinished();
    }
}

void Widget::on_enableselected_clicked()
{
    //启用Play商店
    if(ui->playstore->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm enable com.android.vending");
        process->waitForStarted();
        process->waitForFinished();
    }
    //启用Gboard
    if(ui->gboard->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm enable com.google.android.inputmethod.latin");
        process->waitForStarted();
        process->waitForFinished();
    }
    //启用谷歌播客
    if(ui->podcast->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm enable com.google.android.googlequicksearchbox");
        process->waitForStarted();
        process->waitForFinished();
    }
    //启用GMail
    if(ui->gmail->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm enable com.google.android.gm");
        process->waitForStarted();
        process->waitForFinished();
    }
    //启用Google服务
    if(ui->googleservice->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm enable com.google.android.ims");
        process->waitForStarted();
        process->waitForFinished();
    }
    //启用Google Drive云盘
    if(ui->googledrive->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm enable com.google.android.apps.docs");
        process->waitForStarted();
        process->waitForFinished();
    }
    //启用Google电影（TV）
    if(ui->googlemovie->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm enable com.google.android.videos");
        process->waitForStarted();
        process->waitForFinished();
    }
    //启用Google搜索
    if(ui->googlesearch->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm enable com.google.android.googlequicksearchbox");
        process->waitForStarted();
        process->waitForFinished();
    }
    //启用Google服务框架
    if(ui->googleserviceframework->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm enable com.google.android.gms");
        process->waitForStarted();
        process->waitForFinished();
    }
    //启用Google相册
    if(ui->googlephotos->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm enable com.google.android.apps.photos");
        process->waitForStarted();
        process->waitForFinished();
    }
    //启用Google One服务
    if(ui->googleone->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm enable com.google.android.apps.subscriptions.red");
        process->waitForStarted();
        process->waitForFinished();
    }
    //启用GMap
    if(ui->gmap->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm enable com.google.android.apps.maps");
        process->waitForStarted();
        process->waitForFinished();
    }
    //启用YouTube
    if(ui->youtube->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm enable com.google.android.youtube");
        process->waitForStarted();
        process->waitForFinished();
    }
    //启用Google Pay
    if(ui->googlepay->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm enable com.google.android.apps.nbu.paisa.user");
        process->waitForStarted();
        process->waitForFinished();
    }
    //启用Chrome
    if(ui->chrome->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm enable com.android.chrome");
        process->waitForStarted();
        process->waitForFinished();
    }
    //启用Google Duo
    if(ui->googleduo->isChecked())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell pm enable com.google.android.apps.tachyon");
        process->waitForStarted();
        process->waitForFinished();
    }
}

void Widget::on_reviseresolution_clicked()
{
    QString inputlandscapepixels = ui->landscapepixels->text();
    QString inputportraitpixels = ui->portraitpixels->text();
    if(inputlandscapepixels.isNull() || inputportraitpixels.isNull())   //检测横向像素和纵向像素是否均填写
    {
        qDebug().noquote() << "No Operation";
    }
    else
    {
        QString inputresolution = inputlandscapepixels.append("x").append(inputportraitpixels);
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell wm size" << inputresolution);
        process->waitForStarted();
        process->waitForFinished();
        ui->resolution2->setText("分辨率："+inputresolution);
        QMessageBox notice;
        notice.setWindowTitle("成功！");
        notice.setText("已修改分辨率为" + inputresolution);
        notice.exec();
    }
}

void Widget::on_recoverresolution_clicked()
{
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell wm size reset");
    process->waitForStarted();
    process->waitForFinished();
    //获取屏幕分辨率信息
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell wm size");
    process->waitForStarted();
    process->waitForFinished();
    QString resolution = QString::fromLocal8Bit(process->readAllStandardOutput());
    resolution.replace(QString("Physical size: "), QString(""));    //去除ADB命令输出结果中的"Physical size: "
    ui->resolution2->setText("分辨率："+resolution);
}

void Widget::on_chooseunit_currentTextChanged(const QString &arg1)
{
    if(arg1 == "选择单位")
    {
        ui->inputvalue->setPlaceholderText("请选择单位！");
        ui->inputvalue->setReadOnly(true);
    }
    else
    {
        unit = arg1;    //记录选择的单位
        if(arg1 == "DPI")
        {
            ui->inputvalue->setPlaceholderText("DPI");
            ui->inputvalue->setReadOnly(false);
        }
        if(arg1 == "dp")
        {
            ui->inputvalue->setPlaceholderText("dp");
            ui->inputvalue->setReadOnly(false);
        }
    }
}

void Widget::on_changedpi_clicked()
{
    if(!ui->inputvalue->text().isNull())
    {
        if(unit == "DPI")
        {
            process = new QProcess(this);
            process->start("cmd", QStringList() << "/c" << "adb shell wm density" << ui->inputvalue->text());
            process->waitForStarted();
            process->waitForFinished();
            process = new QProcess(this);
            process->start("cmd", QStringList() << "/c" << "adb shell wm density");
            process->waitForStarted();
            process->waitForFinished();
            QString dpi0 = QString::fromLocal8Bit(process->readAllStandardOutput());
            dpi0.replace(QString("Physical density: "), QString(""));    //去除ADB命令输出结果中的"Physical size: "
            QString dpi;
            if(dpi0.contains("Override density:"))
            {
                QStringList list17 = dpi0.split("Override density:");
                dpi = list17[1];
            }
            else
            {
                dpi = dpi0;
            }
            dpi.replace(QString("Physical density: "), QString(""));    //去除ADB命令输出结果中的"Physical size: "
            ui->DPI->setText("显示密度："+dpi);
            //将DPI换算成dp，原理：dp*ppi/160 = px，其中在屏幕显示中ppi=dpi
            float dpi_float = dpi.toFloat();
            //获取屏幕分辨率信息
            process = new QProcess(this);
            process->start("cmd", QStringList() << "/c" << "adb shell wm size");
            process->waitForStarted();
            process->waitForFinished();
            QString resolution = QString::fromLocal8Bit(process->readAllStandardOutput());
            resolution.replace(QString("Physical size: "), QString(""));    //去除ADB命令输出结果中的"Physical size: "
            QStringList list16 = resolution.split("x");
            QString px = list16[0];
            px.replace("x", "");
            float px_float = px.toFloat();
            float dp_float = px_float*160/dpi_float;
            int dp_integer_float = (int)(dp_float+0.5f);      //对换算得到的dpi数据进行取整处理
            QString dp = QString::number(dp_integer_float);
            ui->DPIanddp->setText("显示密度（DPI）：" + dpi + "\n最小宽度（dp）：" + dp);
        }
        else
        {     
            //将dp换算成DPI，原理：dp*ppi/160 = px，其中在屏幕显示中ppi=dpi
            QString dp = ui->inputvalue->text();
            float dp_float = dp.toFloat();
            //获取屏幕分辨率信息
            process = new QProcess(this);
            process->start("cmd", QStringList() << "/c" << "adb shell wm size");
            process->waitForStarted();
            process->waitForFinished();
            QString resolution = QString::fromLocal8Bit(process->readAllStandardOutput());
            resolution.replace(QString("Physical size: "), QString(""));    //去除ADB命令输出结果中的"Physical size: "
            QStringList list16 = resolution.split("x");
            QString px = list16[0];
            px.replace("x", "");
            float px_float = px.toFloat();
            float dpi_float = px_float*160/dp_float;
            int dpi_integer_float = (int)(dpi_float+0.5f);      //对换算得到的dpi数据进行取整处理
            QString dpi = QString::number(dpi_integer_float);
            process = new QProcess(this);
            process->start("cmd", QStringList() << "/c" << "adb shell wm density" << dpi);
            process->waitForStarted();
            process->waitForFinished();
            ui->DPIanddp->setText("显示密度（DPI）：" + dpi + "\n最小宽度（dp）：" + dp);
        }
    }
}

void Widget::on_recoverdpi_clicked()
{
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell wm density reset");
    process->waitForStarted();
    process->waitForFinished();
    //获取屏幕分辨率信息
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell wm size");
    process->waitForStarted();
    process->waitForFinished();
    QString resolution = QString::fromLocal8Bit(process->readAllStandardOutput());
    //获取DPI信息
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell wm density");
    process->waitForStarted();
    process->waitForFinished();
    QString dpi0 = QString::fromLocal8Bit(process->readAllStandardOutput());
    dpi0.replace(QString("Physical density: "), QString(""));    //去除ADB命令输出结果中的"Physical size: "
    QString dpi;
    if(dpi0.contains("Override density:"))
    {
        QStringList list17 = dpi0.split("Override density:");
        dpi = list17[1];
    }
    else
    {
        dpi = dpi0;
    }
    ui->DPI->setText("显示密度："+dpi);
    //将DPI换算成dp，原理：dp*ppi/160 = px，其中在屏幕显示中ppi=dpi
    float dpi_float = dpi.toFloat();
    resolution.replace(QString("Physical size: "), QString(""));    //去除ADB命令输出结果中的"Physical size: "
    QStringList list16 = resolution.split("x");
    QString px = list16[0];
    px.replace("x", "");
    float px_float = px.toFloat();
    float dp_float = px_float*160/dpi_float;
    int dp_integer_float = (int)(dp_float+0.5f);      //对换算得到的dpi数据进行取整处理
    QString dp = QString::number(dp_integer_float);
    ui->DPIanddp->setText("显示密度（DPI）：" + dpi + "\n最小宽度（dp）：" + dp);
}

void Widget::on_hide_clicked()
{
    QString CommandParameter0;
    if(ui->earphone->isChecked())
    {
        CommandParameter0.append("earphone,");
    }
    if(ui->volte->isChecked())
    {
        CommandParameter0.append("volte_call,");
    }
    if(ui->battery->isChecked())
    {
        CommandParameter0.append("battery,");
    }
    if(ui->time->isChecked())
    {
        CommandParameter0.append("clock,");
    }
    if(ui->signal->isChecked())
    {
        CommandParameter0.append("phone_signal,");
    }
    if(ui->airplane->isChecked())
    {
        CommandParameter0.append("airplane,");
    }
    if(ui->hotspot->isChecked())
    {
        CommandParameter0.append("hotspot,");
    }
    if(ui->cast->isChecked())
    {
        CommandParameter0.append("cast,");
    }
    if(ui->wifi->isChecked())
    {
        CommandParameter0.append("wifi,");
    }
    if(ui->speakerphone->isChecked())
    {
        CommandParameter0.append("speakerphone,");
    }
    if(ui->nfc->isChecked())
    {
        CommandParameter0.append("nfc,");
    }
    if(ui->bluetooth->isChecked())
    {
        CommandParameter0.append("bluetooth,");
    }
    if(ui->alarmclock->isChecked())
    {
        CommandParameter0.append("alarm_clock,");
    }
    if(ui->zen->isChecked())
    {
        CommandParameter0.append("zen,");
    }
    if(ui->powersavingmode->isChecked())
    {
        CommandParameter0.append("powersavingmode,");
    }
    if(ui->mute_2->isChecked())
    {
        CommandParameter0.append("mute,");
    }
    if(ui->volume->isChecked())
    {
        CommandParameter0.append("volume,");
    }
    if(ui->eyesprotect->isChecked())
    {
        CommandParameter0.append("eyes_protect,");
    }
    QString CommandParameter = CommandParameter0.left(CommandParameter0.size()-1);  //去除最后一个“,”
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings put secure icon_blacklist" << CommandParameter);
    process->waitForStarted();
    process->waitForFinished();
}

void Widget::on_recovericon_clicked()
{
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings put secure icon_blacklist null");
    process->waitForStarted();
    process->waitForFinished();
}

void Widget::on_clockseconds_clicked()
{
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << " adb shell settings put secure clock_seconds 1");
    process->waitForStarted();
    process->waitForFinished();
}

void Widget::on_closeclockseconds_2_clicked()
{
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings put secure clock_seconds 0");
    process->waitForStarted();
    process->waitForFinished();
}

void Widget::on_thenumberoficons_currentTextChanged(const QString &arg1)
{
    if(arg1 != "下拉图标个数")
    {
        //将系统版本号与7.0进行比对大小
        std::string MinimumVersion = "7.0";
        std::string CurrentVersion = AndroidVersionNumber.toStdString();
        int n = CurrentVersion.size()>MinimumVersion.size()? CurrentVersion.size():MinimumVersion.size();
        QString state;
        for(int i=0;i<n;)
        {
            if(CurrentVersion[i]>=MinimumVersion[i])
            {
                state = "OK";
                break;
            }
            else
            {
                state = "NO";
                break;
            }
        }
        bool a = (state == "OK");
        if(a == true)
        {
            QString IconNumber;
            if(arg1 == "默认")
            {
                IconNumber = "N";
            }
            else
            {
                IconNumber = arg1;
            }
            process = new QProcess(this);
            process->start("cmd", QStringList() << "/c" << "adb shell settings put secure sysui_qqs_count" << IconNumber);
            process->waitForStarted();
            process->waitForFinished();
            QMessageBox noticemessage;
            noticemessage.setWindowTitle("成功！");
            noticemessage.setText("已完成修改，Android 10+版本的系统修改可能无效，属正常现象");
            noticemessage.exec();
        }
        else
        {
            QMessageBox noticemessage;
            noticemessage.setWindowTitle("失败！");
            noticemessage.setText("该功能只适用于Android 7.0及以上版本！");
            noticemessage.exec();
        }
    }
}

void Widget::on_IconPinSpacing_currentTextChanged(const QString &arg1)
{
    if(arg1 != "图标置顶间距")
    {
        //判断安卓版本是否为Android Pie及以上
        std::string MinimumVersion = "9.0";
        std::string CurrentVersion = AndroidVersionNumber.toStdString();
        int n = CurrentVersion.size()>MinimumVersion.size()? CurrentVersion.size():MinimumVersion.size();
        QString state;
        for(int i=0;i<n;)
        {
            if(CurrentVersion[i]>=MinimumVersion[i])
            {
                state = "OK";
                break;
            }
            else
            {
                state = "NO";
                break;
            }
        }
        bool a = (state == "OK");
        if(a == true)
        {
            QString dp;
            if(arg1 == "默认")
            {
                dp = "null";
            }           
            else
            {
                dp = arg1;
                dp.replace(QString("dp"), QString(""));
            }
            process = new QProcess(this);
            process->start("cmd", QStringList() << "/c" << "adb shell settings put secure sysui_rounded_content_padding" << dp);
            process->waitForStarted();
            process->waitForFinished();
            QMessageBox noticemessage;
            noticemessage.setWindowTitle("成功！");
            noticemessage.setText("已完成修改！");
            noticemessage.exec();
        }
        else
        {
            QMessageBox noticemessage;
            noticemessage.setWindowTitle("失败！");
            noticemessage.setText("该功能只适用于Android Pie (Android 9.0)及以上版本！");
            noticemessage.exec();
        }
    }
}

void Widget::on_RemoveX_clicked()
{
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings put global captive_portal_server g.cn/generate_204");
    process->waitForStarted();
    process->waitForFinished();
    QMessageBox noticemessage;
    noticemessage.setWindowTitle("成功！");
    noticemessage.setText("已去除x或!，若无效请重启设备！");
    noticemessage.exec();
}

void Widget::on_RecoverX_clicked()
{
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings delete global captive_portal_server");
    process->waitForStarted();
    process->waitForFinished();
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings put global captive_portal_detection_enabled 1");
    process->waitForStarted();
    process->waitForFinished();
    QMessageBox noticemessage;
    noticemessage.setWindowTitle("成功！");
    noticemessage.setText("已恢复x或!，若无效请重启设备！");
    noticemessage.exec();
}

void Widget::on_ImmerseStatusBar_clicked()
{
    if(ui->ExcludeApps->text().isNull())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell settings put global policy_control immersive.status=*");
        process->waitForStarted();
        process->waitForFinished();
    }
    else
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell settings put global policy_control immersive.status=" <<ui->ExcludeApps->text());
        process->waitForStarted();
        process->waitForFinished();
    }
}

void Widget::on_ImmerseNavigationBar_clicked()
{
    if(ui->ExcludeApps->text().isNull())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell settings put global policy_control immersive.navigation=*");
        process->waitForStarted();
        process->waitForFinished();
    }
    else
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell settings put global policy_control immersive.navigation=*" <<ui->ExcludeApps->text());
        process->waitForStarted();
        process->waitForFinished();
    }
}
void Widget::on_ImmerseAll_clicked()
{
    if(ui->ExcludeApps->text().isNull())
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell settings put global policy_control immersive.full=*");
        process->waitForStarted();
        process->waitForFinished();
    }
    else
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell settings put global policy_control immersive.full=*" <<ui->ExcludeApps->text());
        process->waitForStarted();
        process->waitForFinished();
    }
}
void Widget::on_RecoverAll_clicked()
{
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings put global policy_control null");
    process->waitForStarted();
    process->waitForFinished();
}

void Widget::on_changetemperat_clicked()
{
    if(!ui->temperatedit->text().isNull())
    {
        QMessageBox confirm;
        confirm.setWindowTitle("确认操作");
        confirm.setText("确定把电池温度修改为" + ui->temperatedit->text() + "℃？");
        confirm.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
        if(confirm.exec() == QMessageBox::No)
        {
            return;
        }
        else
        {
            float float_temperature = ui->temperatedit->text().toFloat();
            QString temperature = QString::number(float_temperature*10);
            process = new QProcess(this);
            process->start("cmd", QStringList() << "/c" << "adb shell dumpsys battery set temp" << temperature);
            process->waitForStarted();
            process->waitForFinished();
            QMessageBox notice;
            notice.setWindowTitle("成功！");
            notice.setText("已将电池温度修改为" + ui->temperatedit->text() + "℃!");
            notice.exec();
        }
    }
}

void Widget::on_changelevel_clicked()
{
    if(!ui->leveledit->text().isNull())
    {
        QMessageBox confirm;
        confirm.setWindowTitle("确认操作");
        confirm.setText("确定把电量修改为" + ui->leveledit->text() + "%？");
        confirm.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
        if(confirm.exec() == QMessageBox::No)
        {
            return;
        }
        else
        {
            process = new QProcess(this);
            process->start("cmd", QStringList() << "/c" << "adb shell dumpsys battery set level" << ui->leveledit->text());
            process->waitForStarted();
            process->waitForFinished();
            QMessageBox notice;
            notice.setWindowTitle("成功！");
            notice.setText("已将电量修改为" + ui->leveledit->text() + "%!");
            notice.exec();
        }
    }
}

void Widget::on_Non_ChargingSwitch_clicked()
{
    if(ui->Non_ChargingSwitch->text() == "开启")
    {
        ui->Non_ChargingSwitch->setText("关闭");
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell dumpsys battery set status 1");
        process->waitForStarted();
        process->waitForFinished();
    }
    else
    {
        ui->Non_ChargingSwitch->setText("开启");
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell dumpsys battery set status 2");
        process->waitForStarted();
        process->waitForFinished();
    }
}

void Widget::on_Wireless_ChargingSwitch_clicked()
{
    if(ui->Wireless_ChargingSwitch->text() == "开启")
    {
        ui->Wireless_ChargingSwitch->setText("关闭");
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell dumpsys battery set wireless 1");
        process->waitForStarted();
        process->waitForFinished();
    }
    else
    {
        ui->Wireless_ChargingSwitch->setText("开启");
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell dumpsys battery set wireless 0");
        process->waitForStarted();
        process->waitForFinished();
    }
}

void Widget::on_USB_ChargingSwitch_clicked()
{
    if(ui->USB_ChargingSwitch->text() == "开启")
    {
        ui->USB_ChargingSwitch->setText("关闭");
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell dumpsys battery set usb 1");
        process->waitForStarted();
        process->waitForFinished();
    }
    else
    {
        ui->USB_ChargingSwitch->setText("开启");
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell dumpsys battery set usb 0");
        process->waitForStarted();
        process->waitForFinished();
    }
}
void Widget::on_AC_ChargingSwitch_clicked()
{
    if(ui->AC_ChargingSwitch->text() == "开启")
    {
        ui->AC_ChargingSwitch->setText("关闭");
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell dumpsys battery set ac 1");
        process->waitForStarted();
        process->waitForFinished();
    }
    else
    {
        ui->AC_ChargingSwitch->setText("开启");
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell dumpsys battery set ac 0");
        process->waitForStarted();
        process->waitForFinished();
    }
}


void Widget::on_ResetBatteryInfoAndStatus_clicked()
{
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell dumpsys battery reset");
    process->waitForStarted();
    process->waitForFinished();
}

void Widget::on_comboBox_currentTextChanged(const QString &arg1)
{
    //判断用户是否选择间隔时间
    if(arg1.contains("s") || ((!arg1.contains("s")) && (ui->StartBatteryCounting->text() == "停止")))
    {
        ui->StartBatteryCounting->setEnabled(true);
        QString time = arg1;
        CountingTime = (time.replace(QString("s"), QString("000"))).toFloat();
    }
    else
    {
        ui->StartBatteryCounting->setEnabled(false);
    }
}

void Widget::on_StartBatteryCounting_clicked()
{
    if(ui->StartBatteryCounting->text() == "开始")
    {
        ui->Time->setText(time.toString("hh:mm:ss"));
        //获取电池信息
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb shell dumpsys battery");
        process->waitForStarted();
        process->waitForFinished();
        QString batteryinfo = QString::fromLocal8Bit(process->readAllStandardOutput());
        //获取电量百分比信息
        QStringList list0 = batteryinfo.split("level:");
        QString betteryinfo0 = list0[1];   //初步对电池信息按需求（电量百分比、电压、温度）进行切割
        QStringList list1 = betteryinfo0.split("scale");
        QString level = list1[0];
        level.replace(QString(" "), QString(""));   //电量百分比数字
        level.replace(QString("\n"), QString(""));      //去掉ADB调试输出电量百分数数据后的换行
        level.replace(QString("\r"),QString(""));   //去掉数据的回车
        ui->Level->setText(level);
    }
}

void Widget::on_FontScaling_valueChanged(int value)
{
    ui->FontScalingShower->clear();
    float float_value = value;
    ZoomMultipliter = QString::number(float_value/100);
    ui->FontScalingShower->setText("设定倍数：" + ZoomMultipliter + "x");
    //获取当前倍数
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings get system font_scale");
    process->waitForStarted();
    process->waitForFinished();
    CurrentFontZoomMultipliter = QString::fromLocal8Bit(process->readAllStandardOutput());
    CurrentFontZoomMultipliter.replace(QString("\r"), QString(""));
    CurrentFontZoomMultipliter.replace(QString("\n"), QString(""));
    ui->FontScalingShower->append("当前倍数：" + CurrentFontZoomMultipliter + "x");
}

void Widget::on_FontScaling_sliderReleased()
{
    ui->FontScalingShower->clear();
    //修改倍数
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings put system font_scale" << ZoomMultipliter);
    process->waitForStarted();
    process->waitForFinished();
    //获取当前倍数
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings get system font_scale");
    process->waitForStarted();
    process->waitForFinished();
    CurrentFontZoomMultipliter = QString::fromLocal8Bit(process->readAllStandardOutput());
    CurrentFontZoomMultipliter.replace(QString("\r"), QString(""));
    CurrentFontZoomMultipliter.replace(QString("\n"), QString(""));
    ui->FontScalingShower->setText("设定倍数：" + CurrentFontZoomMultipliter + "x");
    ui->FontScalingShower->append("当前倍数：" + CurrentFontZoomMultipliter + "x");
}

void Widget::on_WindowAnimation_valueChanged(int value)
{
    float float_value = value;
    WindowAnimationValue = QString::number(float_value/100);
    ui->AnimationSpeed->clear();
    //将动画速度相关参数的初始值打印到文本框上
    ui->AnimationSpeed->setText("窗口速度：" + WindowAnimationValue + "x");
    ui->AnimationSpeed->append("过渡速度：" + CurrentTransitionAnimations + "x");
    ui->AnimationSpeed->append("程序速度：" + CurrentAnimatorDuration + "x");
}

void Widget::on_WindowAnimation_sliderReleased()
{
    //修改窗口动画速度
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings put global window_animation_scale" << WindowAnimationValue);
    process->waitForStarted();
    process->waitForFinished();
    //分别获取当前窗口、过渡和程序动画速度
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings get global window_animation_scale");
    process->waitForStarted();
    process->waitForFinished();
    CurrentWindowAnimation = QString::fromLocal8Bit(process->readAllStandardOutput());
    CurrentWindowAnimation.remove(QString("\r") + QString("\n"));
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings get global transition_animation_scale");
    process->waitForStarted();
    process->waitForFinished();
    CurrentTransitionAnimations = QString::fromLocal8Bit(process->readAllStandardOutput());
    CurrentTransitionAnimations.remove(QString("\r") + QString("\n"));
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings get global animator_duration_scale");
    process->waitForStarted();
    process->waitForFinished();
    CurrentAnimatorDuration = QString::fromLocal8Bit(process->readAllStandardOutput());
    CurrentAnimatorDuration.remove(QString("\r") + QString("\n"));
    ui->AnimationSpeed->clear();
    //将动画速度相关参数的初始值打印到文本框上
    ui->AnimationSpeed->setText("窗口速度：" + CurrentWindowAnimation + "x");
    ui->AnimationSpeed->append("过渡速度：" + CurrentTransitionAnimations + "x");
    ui->AnimationSpeed->append("程序速度：" + CurrentAnimatorDuration + "x");
}

void Widget::on_TransitionAnimations_valueChanged(int value)
{
    float float_value = value;
    TransitionAnimationsValue = QString::number(float_value/100);
    ui->AnimationSpeed->clear();
    //将动画速度相关参数的初始值打印到文本框上
    ui->AnimationSpeed->setText("窗口速度：" + CurrentWindowAnimation + "x");
    ui->AnimationSpeed->append("过渡速度：" + TransitionAnimationsValue + "x");
    ui->AnimationSpeed->append("程序速度：" + CurrentAnimatorDuration + "x");
}

void Widget::on_TransitionAnimations_sliderReleased()
{
    //修改过渡动画速度
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings put global transition_animation_scale" << TransitionAnimationsValue);
    process->waitForStarted();
    process->waitForFinished();
    //分别获取当前窗口、过渡和程序动画速度
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings get global window_animation_scale");
    process->waitForStarted();
    process->waitForFinished();
    CurrentWindowAnimation = QString::fromLocal8Bit(process->readAllStandardOutput());
    CurrentWindowAnimation.remove(QString("\r") + QString("\n"));
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings get global transition_animation_scale");
    process->waitForStarted();
    process->waitForFinished();
    CurrentTransitionAnimations = QString::fromLocal8Bit(process->readAllStandardOutput());
    CurrentTransitionAnimations.remove(QString("\r") + QString("\n"));
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings get global animator_duration_scale");
    process->waitForStarted();
    process->waitForFinished();
    CurrentAnimatorDuration = QString::fromLocal8Bit(process->readAllStandardOutput());
    CurrentAnimatorDuration.remove(QString("\r") + QString("\n"));
    ui->AnimationSpeed->clear();
    //将动画速度相关参数的初始值打印到文本框上
    ui->AnimationSpeed->setText("窗口速度：" + CurrentWindowAnimation + "x");
    ui->AnimationSpeed->append("过渡速度：" + CurrentTransitionAnimations + "x");
    ui->AnimationSpeed->append("程序速度：" + CurrentAnimatorDuration + "x");
}

void Widget::on_AnimatorDuration_valueChanged(int value)
{
    float float_value = value;
    AnimatorDurationValue = QString::number(float_value/100);
    ui->AnimationSpeed->clear();
    //将动画速度相关参数的初始值打印到文本框上
    ui->AnimationSpeed->setText("窗口速度：" + CurrentWindowAnimation + "x");
    ui->AnimationSpeed->append("过渡速度：" + CurrentTransitionAnimations + "x");
    ui->AnimationSpeed->append("程序速度：" + AnimatorDurationValue + "x");
}

void Widget::on_AnimatorDuration_sliderReleased()
{
    //修改程序动画速度
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings put global animator_duration_scale" << AnimatorDurationValue);
    process->waitForStarted();
    process->waitForFinished();
    //分别获取当前窗口、过渡和程序动画速度
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings get global window_animation_scale");
    process->waitForStarted();
    process->waitForFinished();
    CurrentWindowAnimation = QString::fromLocal8Bit(process->readAllStandardOutput());
    CurrentWindowAnimation.remove(QString("\r") + QString("\n"));
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings get global transition_animation_scale");
    process->waitForStarted();
    process->waitForFinished();
    CurrentTransitionAnimations = QString::fromLocal8Bit(process->readAllStandardOutput());
    CurrentTransitionAnimations.remove(QString("\r") + QString("\n"));
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings get global animator_duration_scale");
    process->waitForStarted();
    process->waitForFinished();
    CurrentAnimatorDuration = QString::fromLocal8Bit(process->readAllStandardOutput());
    CurrentAnimatorDuration.remove(QString("\r") + QString("\n"));
    ui->AnimationSpeed->clear();
    //将动画速度相关参数的初始值打印到文本框上
    ui->AnimationSpeed->setText("窗口速度：" + CurrentWindowAnimation + "x");
    ui->AnimationSpeed->append("过渡速度：" + CurrentTransitionAnimations + "x");
    ui->AnimationSpeed->append("程序速度：" + CurrentAnimatorDuration + "x");
}

void Widget::on_ChooseTimeUnit_currentTextChanged(const QString &arg1)
{
    if(arg1 == "单位选择")
    {
        ui->ScreenOff->setPlaceholderText("请选择单位！");
        ui->ScreenOff->setEnabled(false);
        ui->ChangeScreenOffTime->setEnabled(false);
    }
    else
    {
        TimeUnit = arg1;
        ui->ScreenOff->setEnabled(true);
        ui->ScreenOff->setPlaceholderText("请输入自动锁屏时间（" + arg1 + "）");
        if(!ui->ScreenOff->text().isEmpty())
        {
            ui->ChangeScreenOffTime->setEnabled(true);
        }
    }
}

void Widget::on_ScreenOff_textEdited(const QString &arg1)
{
    if(arg1.isEmpty())
    {
        ui->ChangeScreenOffTime->setEnabled(false);
    }
    else
    {
        ui->ChangeScreenOffTime->setEnabled(true);
    }
}

void Widget::on_ChangeScreenOffTime_clicked()
{
    QString time;
    if(TimeUnit == "min")
    {
        float float_time = ui->ScreenOff->text().toFloat();
        time = QString::number(float_time*60);
    }
    else
    {
        time = ui->ScreenOff->text();
    }
    time.append("000");     //ADB命令中时间的单位为毫秒
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "adb shell settings put system screen_off_timeout" << time);
    process->waitForStarted();
    process->waitForFinished();
}

void Widget::on_InputADBCommand_returnPressed()
{
    QString ADBCommand = ui->InputADBCommand->text();
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << ADBCommand);
    process->waitForStarted();
    process->waitForFinished();
    QString ADBOutput = QString::fromLocal8Bit(process->readAllStandardOutput());
    ui->ADBOutputBrowser->setText(ADBOutput);
}

void Widget::on_UnlockBootloader_clicked()
{
    //校验是否已连接Fastboot设备
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "fastboot device");
    process->waitForStarted();
    process->waitForFinished();
    if(QString::fromLocal8Bit(process->readAllStandardOutput()).isEmpty())
    {
        QMessageBox notice0;
        notice0.setWindowTitle("错误！");
        notice0.setText("请确保Fastboot驱动已正确安装，同时保证有且仅有一部Fastboot设备正确连接并重试！");
        notice0.exec();
        return;
    }
    else
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "fastboot flashing unlock");
        process->waitForStarted();
        process->waitForFinished();
        QMessageBox notice1;
        notice1.setWindowTitle("已完成操作！");
        notice1.setText("该功能仅执行解锁Bootloader的通用命令，部分品牌机型可能无效，若无效请以具体解锁方式操作！");
        notice1.exec();
    }
}

void Widget::on_LockBootloader_clicked()
{
    //校验是否已连接Fastboot设备
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "fastboot device");
    process->waitForStarted();
    process->waitForFinished();
    if(QString::fromLocal8Bit(process->readAllStandardOutput()).isEmpty())
    {
        QMessageBox notice0;
        notice0.setWindowTitle("错误！");
        notice0.setText("请确保Fastboot驱动已正确安装，同时保证有且仅有一部Fastboot设备正确连接并重试！");
        notice0.exec();
        return;
    }
    else
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "fastboot flashing unlock");
        process->waitForStarted();
        process->waitForFinished();
        QMessageBox notice1;
        notice1.setWindowTitle("已完成操作！");
        notice1.setText("该功能仅执行回锁Bootloader的通用命令，部分品牌机型可能无效，若无效请以具体回锁方式操作！");
        notice1.exec();
    }
}

void Widget::on_FlashBoot_clicked()
{
    QMessageBox confirm;
    confirm.setWindowTitle("注意！");
    confirm.setText("请确保设备已进入Fastboot模式并且有且仅有1部设备成功通过USB调试模式连接至电脑，如已确保请继续！");
    confirm.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    if(confirm.exec() == QMessageBox::No)
    {
        return;
    }
    else
    {
        //选择镜像文件
        QString PathName = QFileDialog::getOpenFileName(this, "请选择要刷入的boot.img镜像文件", "C:/", "*img");
        if(PathName.isNull())
        {
            return;
        }
        else
        {
            QFileInfo fileinfo = QFileInfo(PathName);
            QString CopyOfFile = fileinfo.fileName();
            QString TargetPath = "./imgs/" + CopyOfFile;
            if(QFile::exists(TargetPath))
            {
                QFile::remove(TargetPath);
            }
            QFile::copy(PathName, TargetPath);
            process = new QProcess(this);
            process->start("cmd", QStringList() << "/c" << "fastboot flash boot" << TargetPath);
            process->waitForStarted();
            process->waitForFinished();
            QString Output = QString::fromLocal8Bit(process->readAllStandardOutput());
            if(Output.isEmpty())
            {
                QMessageBox notice0;
                notice0.setWindowTitle("失败！");
                notice0.setText("刷入失败，请确保电脑已正确安装Fastboot驱动且已正确连接Fastboot设备！");
                notice0.exec();
            }
            else if(Output.contains("failed"))
            {
                QMessageBox notice1;
                notice1.setWindowTitle("失败！");
                notice1.setText("刷入失败,请确保选择正确的boot镜像并重试！");
                notice1.exec();
            }
            else
            {
                QMessageBox notice2;
                notice2.setWindowTitle("成功！");
                notice2.setText("已成功刷入boot镜像！");
            }
            QFile::remove(TargetPath);
        }
    }
}

void Widget::on_FlashVbmeta_clicked()
{
    QMessageBox confirm;
    confirm.setWindowTitle("注意！");
    confirm.setText("请确保设备已进入Fastboot模式并且有且仅有1部设备成功通过USB调试模式连接至电脑，如已确保请继续！");
    confirm.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    if(confirm.exec() == QMessageBox::No)
    {
        return;
    }
    else
    {
        //选择镜像文件
        QString PathName = QFileDialog::getOpenFileName(this, "请选择用于破除开机验证的vbmeta.img镜像文件", "C:/", "*img");
        if(PathName.isNull())
        {
            return;
        }
        else
        {
            QFileInfo fileinfo = QFileInfo(PathName);
            QString CopyOfFile = fileinfo.fileName();
            QString TargetPath = "./imgs/" + CopyOfFile;
            if(QFile::exists(TargetPath))
            {
                QFile::remove(TargetPath);
            }
            QFile::copy(PathName, TargetPath);
            process = new QProcess(this);
            process->start("cmd", QStringList() << "/c" << "fastboot --disable-verity --disable-verification flash vbmeta" << TargetPath);
            process->waitForStarted();
            process->waitForFinished();
            QString Output = QString::fromLocal8Bit(process->readAllStandardOutput());
            if(Output.isEmpty())
            {
                QMessageBox notice0;
                notice0.setWindowTitle("失败！");
                notice0.setText("刷入失败，请确保电脑已正确安装Fastboot驱动且已正确连接Fastboot设备！");
                notice0.exec();
            }
            else if(Output.contains("failed"))
            {
                QMessageBox notice1;
                notice1.setWindowTitle("失败！");
                notice1.setText("刷入失败,请确保选择正确的vbmeta镜像并重试！");
                notice1.exec();
            }
            else
            {
                QMessageBox notice2;
                notice2.setWindowTitle("成功！");
                notice2.setText("已成功刷入vbmeta镜像！");
            }
            QFile::remove(TargetPath);
        }
    }
}

void Widget::on_FlashRecovery_clicked()
{
    QMessageBox confirm;
    confirm.setWindowTitle("注意！");
    confirm.setText("请确保设备已进入Fastboot模式并且有且仅有1部设备成功通过USB调试模式连接至电脑，如已确保请继续！");
    confirm.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    if(confirm.exec() == QMessageBox::No)
    {
        return;
    }
    else
    {
        //选择镜像文件
        QString PathName = QFileDialog::getOpenFileName(this, "请选择要刷入的recovery.img镜像文件", "C:/", "*img");
        if(PathName.isNull())
        {
            return;
        }
        else
        {
            QFileInfo fileinfo = QFileInfo(PathName);
            QString CopyOfFile = fileinfo.fileName();
            QString TargetPath = "./imgs/" + CopyOfFile;
            if(QFile::exists(TargetPath))
            {
                QFile::remove(TargetPath);
            }
            QFile::copy(PathName, TargetPath);
            process = new QProcess(this);
            process->start("cmd", QStringList() << "/c" << "fastboot flash recovery" << TargetPath);
            process->waitForStarted();
            process->waitForFinished();
            QString Output = QString::fromLocal8Bit(process->readAllStandardOutput());
            if(Output.isEmpty())
            {
                QMessageBox notice0;
                notice0.setWindowTitle("失败！");
                notice0.setText("刷入失败，请确保电脑已正确安装Fastboot驱动且已正确连接Fastboot设备！");
                notice0.exec();
            }
            else if(Output.contains("failed"))
            {
                QMessageBox notice1;
                notice1.setWindowTitle("失败！");
                notice1.setText("刷入失败,请确保选择正确的recovery镜像并重试！");
                notice1.exec();
            }
            else
            {
                QMessageBox notice2;
                notice2.setWindowTitle("成功！");
                notice2.setText("已成功刷入recovery镜像！");
            }
            QFile::remove(TargetPath);
        }
    }
}

void Widget::on_SideloadOTA_clicked()
{
    QMessageBox confirm;
    confirm.setWindowTitle("注意！");
    confirm.setText("请确保设备已正确进入ADB Sideload模式，如已确保请继续！");
    confirm.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    if(confirm.exec() == QMessageBox::No)
    {
        return;
    }
    else
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "adb devices");
        process->waitForStarted();
        process->waitForFinished();
        if(QString::fromLocal8Bit(process->readAllStandardOutput()) == "List of devices attached\r\n\r\n")
        {
            QMessageBox notice0;
            notice0.setWindowTitle("错误！");
            notice0.setText("请确保设备已连接！");
            notice0.exec();
            return;
        }
        else
        {
            //选择ROM文件
            QString PathName = QFileDialog::getOpenFileName(this, "请选择要刷入的ROM文件", "C:/", "*zip");
            if(PathName.isNull())
            {
                return;
            }
            else
            {
                QFileInfo fileinfo = QFileInfo(PathName);
                QString CopyOfFile = fileinfo.fileName();
                QString TargetPath = "./imgs/" + CopyOfFile;
                if(QFile::exists(TargetPath))
                {
                    QFile::remove(TargetPath);
                }
                QFile::copy(PathName, TargetPath);
                process = new QProcess(this);
                process->start("cmd", QStringList() << "/c" << "adb sideload" << TargetPath);
                process->waitForStarted();
                process->waitForFinished();
                QString Output = QString::fromLocal8Bit(process->readAllStandardOutput());
                if(Output.contains("Total"))
                {
                    QMessageBox notice0;
                    notice0.setWindowTitle("成功！");
                    notice0.setText("已成功刷入ROM包！");
                    notice0.exec();
                }
                else
                {
                    QMessageBox notice1;
                    notice1.setWindowTitle("失败！");
                    notice1.setText("未能成功刷入ROM包，请重试！");
                    return;
                }
                QFile::remove(TargetPath);
            }
        }
    }
}

void Widget::on_rebootrecovery_2_clicked()
{
    QStringList args;
        args.append("reboot");
        args.append("recovey");
    process->startDetached(".\\adb.exe",args);
}

void Widget::on_rebootfastboot_2_clicked()
{
    QStringList args;
        args.append("reboot");
        args.append("bootloader");
    process->startDetached(".\\adb.exe",args);
}

void Widget::on_reboot_2_clicked()
{
    //process = new QProcess();   //创建进程
        QStringList args;
            args.append("reboot");
    process->startDetached(".\\adb.exe",args);
}

void Widget::on_poweroff_2_clicked()
{
    QStringList args;
        args.append("shell");
        args.append("reboot");
        args.append("-p");
    process->startDetached(".\\adb.exe",args);
}

void Widget::on_FastbootRebootRecovery_clicked()
{
    QStringList args;
        args.append("reboot");
        args.append("recovery");
    process->startDetached(".\\fastboot.exe",args);
}

void Widget::on_FastbootReboot_clicked()
{
    QStringList args;
        args.append("reboot");
    process->startDetached(".\\fastboot.exe",args);
}

void Widget::on_adbreboot9008_2_clicked()
{
    QStringList args;
        args.append("reboot");
        args.append("edl");
    process->startDetached(".\\adb.exe",args);
}

void Widget::on_fastbootreboot9008_2_clicked()
{
    QStringList args;
        args.append("oem");
        args.append("edl");
    process->startDetached(".\\fastboot.exe",args);
}

void Widget::on_TemporaryIMG_clicked()
{
    QMessageBox confirm;
    confirm.setWindowTitle("注意！");
    confirm.setText("请确保设备已进入Fastboot模式并且有且仅有1部设备成功通过USB调试模式连接至电脑，如已确保请继续！");
    confirm.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    if(confirm.exec() == QMessageBox::No)
    {
        return;
    }
    else
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "fastboot device");
        process->waitForStarted();
        process->waitForFinished();
        if(QString::fromLocal8Bit(process->readAllStandardOutput()).isEmpty())
        {
            QMessageBox notice0;
            notice0.setWindowTitle("错误！");
            notice0.setText("请确保设备已连接！");
            notice0.exec();
            return;
        }
        else
        {
            //选择镜像文件
            QString PathName = QFileDialog::getOpenFileName(this, "请选择要刷入的临时镜像（临时Twrp）文件", "C:/", "*img");
            if(PathName.isNull())
            {
                return;
            }
            else
            {
                QFileInfo fileinfo = QFileInfo(PathName);
                QString CopyOfFile = fileinfo.fileName();
                QString TargetPath = "./imgs/" + CopyOfFile;
                if(QFile::exists(TargetPath))
                {
                    QFile::remove(TargetPath);
                }
                QFile::copy(PathName, TargetPath);
                process = new QProcess(this);
                process->start("cmd", QStringList() << "/c" << "fastboot boot" << TargetPath);
                process->waitForStarted();
                process->waitForFinished();
                QMessageBox notice;
                notice.setWindowTitle("成功！");
                notice.setText("已成功部署临时镜像！");
                notice.exec();
                QFile::remove(TargetPath);
            }
        }
    }
}

void Widget::on_SetActivea_clicked()
{
    QMessageBox confirm;
    confirm.setWindowTitle("二次确认！");
    confirm.setText("确认更改至a分区？");
    confirm.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    if(confirm.exec() == QMessageBox::No)
    {
        return;
    }
    else
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "fastboot device");
        process->waitForStarted();
        process->waitForFinished();
        if(QString::fromLocal8Bit(process->readAllStandardOutput()).isEmpty())
        {
            QMessageBox notice0;
            notice0.setWindowTitle("错误！");
            notice0.setText("请确保设备已连接！");
            notice0.exec();
            return;
        }
        else
        {
            process = new QProcess(this);
            process->start("cmd", QStringList() << "/c" << "fastboot set_active a");
            process->waitForStarted();
            process->waitForFinished();
            QMessageBox notice;
            notice.setWindowTitle("成功！");
            notice.setText("已成功更改至a分区！");
            notice.exec();
        }
    }
}

void Widget::on_SetActiveb_clicked()
{
    process = new QProcess(this);
    process->start("cmd", QStringList() << "/c" << "fastboot device");
    process->waitForStarted();
    process->waitForFinished();
    if(QString::fromLocal8Bit(process->readAllStandardOutput()).isEmpty())
    {
        QMessageBox notice0;
        notice0.setWindowTitle("错误！");
        notice0.setText("请确保设备已连接！");
        notice0.exec();
        return;
    }
    else
    {
        QMessageBox confirm;
        confirm.setWindowTitle("二次确认！");
        confirm.setText("确认更改至b分区？");
        confirm.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
        if(confirm.exec() == QMessageBox::No)
        {
            return;
        }
        else
        {
            process = new QProcess(this);
            process->start("cmd", QStringList() << "/c" << "fastboot set_active b");
            process->waitForStarted();
            process->waitForFinished();
            QMessageBox notice;
            notice.setWindowTitle("成功！");
            notice.setText("已成功更改至b分区！");
            notice.exec();
        }
    }
}

void Widget::on_ChoosePartition_currentTextChanged(const QString &arg1)
{
    Partition = arg1;
    if(Partition == "请选择分区")
    {
        //设置分区操作按钮均为不可用状态
        ui->ErasePartition->setEnabled(false);
        ui->FormatPartition->setEnabled(false);
        ui->FlashPartition->setEnabled(false);
    }
    else if(Partition == "所有分区")
    {
        ui->ErasePartition->setEnabled(true);
        ui->FormatPartition->setEnabled(false);
        ui->FlashPartition->setEnabled(true);
    }
    else
    {
        //设置分区操作按钮均为可用状态
        ui->ErasePartition->setEnabled(true);
        ui->FormatPartition->setEnabled(true);
        ui->FlashPartition->setEnabled(true);
    }
}

void Widget::on_ErasePartition_clicked()
{
    QString ChoosedPartition;
    if(Partition == "所有分区")
    {
        ChoosedPartition = "-w";
    }
    else
    {
        ChoosedPartition = Partition;
    }
    QMessageBox confirm;
    confirm.setWindowTitle("二次确认！");
    confirm.setText("确认进行擦除分区操作？");
    confirm.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    if(confirm.exec() == QMessageBox::No)
    {
        return;
    }
    else
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "fastboot device");
        process->waitForStarted();
        process->waitForFinished();
        if(QString::fromLocal8Bit(process->readAllStandardOutput()).isEmpty())
        {
            QMessageBox notice0;
            notice0.setWindowTitle("错误！");
            notice0.setText("请确保设备已连接！");
            notice0.exec();
            return;
        }
        else
        {
            process = new QProcess(this);
            process->start("cmd", QStringList() << "/c" << "fastboot erase" << ChoosedPartition);
            process->waitForStarted();
            process->waitForFinished();
            QMessageBox notice;
            notice.setWindowTitle("成功！");
            notice.setText("已成功擦除" + ChoosedPartition +"分区！");
            notice.exec();
        }
    }
}

void Widget::on_FormatPartition_clicked()
{
    QMessageBox confirm;
    confirm.setWindowTitle("二次确认！");
    confirm.setText("确认进行格式化分区操作？");
    confirm.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    if(confirm.exec() == QMessageBox::No)
    {
        return;
    }
    else
    {
        process = new QProcess(this);
        process->start("cmd", QStringList() << "/c" << "fastboot device");
        process->waitForStarted();
        process->waitForFinished();
        if(QString::fromLocal8Bit(process->readAllStandardOutput()).isEmpty())
        {
            QMessageBox notice0;
            notice0.setWindowTitle("错误！");
            notice0.setText("请确保设备已连接！");
            notice0.exec();
            return;
        }
        else
        {
            process = new QProcess(this);
            process->start("cmd", QStringList() << "/c" << "fastboot format" << Partition);
            process->waitForStarted();
            process->waitForFinished();
            QMessageBox notice;
            notice.setWindowTitle("成功！");
            notice.setText("已成功格式化分区！");
            notice.exec();
        }
    }
}

void Widget::on_FlashPartition_clicked()
{
    if(Partition == "所有分区")
    {
        QMessageBox confirm;
        confirm.setWindowTitle("注意！");
        confirm.setText("该功能并非刷入所有分区，以用户所选的img镜像文件为准，选择了什么img就刷入对应的分区");
        confirm.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
        if(confirm.exec() == QMessageBox::No)
        {
            return;
        }
        else
        {
            QString SelectedIMGFile = "确定刷入如下镜像？\n";
            QStringList fileList = QFileDialog::getOpenFileNames(this, "请选择要刷入的img镜像文件", "C:/", "*img");    //多选文件
            QStringList TargetPathList;
            if(fileList.count()>0)      //判断用户是否选择文件
            {
                for(int i=0;i<fileList.count();i++)     //for循环复制用户所选文件并录入文件信息
                {
                    TargetPathList << "./" + QFileInfo(fileList[i]).fileName();
                    if(QFile::exists(TargetPathList[i]))
                    {
                        QFile::remove(TargetPathList[i]);
                    }
                    QFile::copy(fileList[i], TargetPathList[i]);
                    SelectedIMGFile.append(QFileInfo(fileList[i]).fileName());
                    SelectedIMGFile.append("\n");
                }
                SelectedIMGFile.replace(QRegularExpression("\n$"), "");
                QMessageBox SecondConfirmation;
                SecondConfirmation.setWindowTitle("二次确认！");
                SecondConfirmation.setText(SelectedIMGFile);
                SecondConfirmation.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
                if(SecondConfirmation.exec() == QMessageBox::No)
                {
                    return;
                }
                else
                {
                    process = new QProcess(this);
                    process->start("cmd", QStringList() << "/c" << "fastboot device");
                    process->waitForStarted();
                    process->waitForFinished();
                    if(QString::fromLocal8Bit(process->readAllStandardOutput()).isEmpty())
                    {
                        QMessageBox notice0;
                        notice0.setWindowTitle("错误！");
                        notice0.setText("请确保设备已连接！");
                        notice0.exec();
                    }
                    else
                    {
                        process = new QProcess(this);
                        process->start("cmd", QStringList() << "/c" << "fastboot flashall");
                        process->waitForStarted();
                        process->waitForFinished();
                        QString Output = QString::fromLocal8Bit(process->readAllStandardOutput());
                        QMessageBox notice1;
                        notice1.setWindowTitle("已完成！");
                        notice1.setText("操作已执行！命令输出已打印在文本框内！");
                        notice1.exec();
                        ui->CommandOutput->append(Output);
                        for(int i=0;i<TargetPathList.count();i++)   //删除文件
                        {
                            if(QFile::exists(TargetPathList[i]))
                            {
                                QFile::remove(TargetPathList[i]);
                            }
                        }
                    }
                }
            }
            else
            {
                return;
            }
        }
    }
    else
    {
        QString PathName = QFileDialog::getOpenFileName(this, "请选择要刷入的分区对应的镜像文件", "C:/", "*img");
        if(PathName.isNull())
        {
            return;
        }
        else
        {
            QMessageBox confirm;
            confirm.setWindowTitle("二次确认");
            confirm.setText("确定刷入" + QFileInfo(PathName).fileName() + "？");
            confirm.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
            if(confirm.exec() == QMessageBox::No)
            {
                return;
            }
            else
            {
                process = new QProcess(this);
                process->start("cmd", QStringList() << "/c" << "fastboot device");
                process->waitForStarted();
                process->waitForFinished();
                if(QString::fromLocal8Bit(process->readAllStandardOutput()).isEmpty())
                {
                    QMessageBox notice0;
                    notice0.setWindowTitle("错误！");
                    notice0.setText("请确保设备已连接！");
                    notice0.exec();
                    return;
                }
                else
                {
                    QFileInfo fileinfo = QFileInfo(PathName);
                    QString CopyOfFile = fileinfo.fileName();
                    QString TargetPath = "./imgs/" + CopyOfFile;
                    if(QFile::exists(TargetPath))
                    {
                        QFile::remove(TargetPath);
                    }
                    QFile::copy(PathName, TargetPath);
                    process = new QProcess(this);
                    process->start("cmd", QStringList() << "/c" << "fastboot flash" << Partition <<TargetPath);
                    process->waitForStarted();
                    process->waitForFinished();
                    QString Output = QString::fromLocal8Bit(process->readAllStandardOutput());
                    QMessageBox notice1;
                    notice1.setWindowTitle("已完成！");
                    notice1.setText("操作已执行！命令输出已打印在文本框内！");
                    notice1.exec();
                    ui->CommandOutput->append(Output);
                    if(QFile::exists(TargetPath))
                    {
                        QFile::remove(TargetPath);
                    }
                }
            }
        }
    }
}

void Widget::on_BilibiliLink_clicked()
{
    QString URL = "https://space.bilibili.com/424226681?spm_id_from=333.1007.0.0";
    QDesktopServices::openUrl(QUrl(URL.toLatin1()));
}
void Widget::on_CoolapkLink_clicked()
{
    QString URL = "http://www.coolapk.com/u/6195291";
    QDesktopServices::openUrl(QUrl(URL.toLatin1()));
}

void Widget::on_FeedbackProblems_clicked()
{
    QMessageBox FeedbackEmail;
    FeedbackEmail.setWindowTitle("问题反馈邮箱");
    FeedbackEmail.setText("如在使用本软件的过程中发现问题，请反馈至该邮箱：cn-scars@qq.com");
    FeedbackEmail.exec();
}

void Widget::on_DownloadOldVersionSoftware_clicked()
{
    QMessageBox notice;
    notice.setWindowTitle("错误！");
    notice.setText("暂无历史版本！");
    notice.exec();
}
