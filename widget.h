#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QProcess>
#include <QTime>
#include<QtNetwork/QNetworkAccessManager>
QT_BEGIN_NAMESPACE
class QFile;
class QSslError;
class QAuthenticator;
class QNetworkReply;
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    QProcess *process;  //定义进程指针
    QString TabPageNumber;  //定义TabWidget的页数值
    QString unit;   //定义用于存储屏幕密度修改所选择的单位
    QTimer* timer;
    QString DeviceSerialNumber;
    QString AndroidVersionNumber;
    QTime time;
    QString CurrentFontZoomMultipliter;
    QString ZoomMultipliter;
    QString CurrentWindowAnimation;
    QString CurrentTransitionAnimations;
    QString CurrentAnimatorDuration;
    QString WindowAnimationValue;
    QString TransitionAnimationsValue;
    QString AnimatorDurationValue;
    QString ScreenOffTime;
    QString TimeUnit;
    QString Partition;
    QString CurrentVersion = "V0.1";    //定义当前软件的版本号
    QString LatestVersion;
    QString Url;
    QString UpdateTime;
    QString ReleaseNote;

    float CountingTime;       
    float float_CurrentScreenOffTime;

    void startRequest(const QUrl &requestedUrl);
    void ParseJSON(bool a);     //JSON解析槽函数

    int VersionCompare(const QString &Version1, const QString &Version2);   //版本号比较函数

private slots:

    void on_reboot_clicked();

    void on_rebootrecovery_clicked();

    void on_rebootfastboot_clicked();

    void on_poweroff_clicked();

    void on_adbreboot9008_clicked();

    void on_fastbootreboot9008_clicked();

    void on_back_clicked();

    void on_home_clicked();

    void on_menu_clicked();

    void on_power_clicked();

    void on_screenshot_clicked();

    void on_tasks_clicked();

    void on_volumeup_clicked();

    void on_volumedown_clicked();

    void on_mute_clicked();

    void on_brightnessup_clicked();

    void on_brightnessdown_clicked();

    void on_voiceassistant_clicked();

    void on_disable_clicked();

    void on_enable_clicked();

    void on_extractAPK_clicked();

    void on_openAPKfolder_clicked();

    void on_getnameandactivity_clicked();

    void on_install_clicked();

    void on_uninstallkeepdata_clicked();

    void on_uninstall_clicked();

    void on_forcestop_clicked();

    void on_cleardata_clicked();

    void on_appinfo_clicked();

    void on_disableselected_clicked();

    void on_enableselected_clicked();

    void on_sendfile_clicked();

    void on_reviseresolution_clicked();

    void on_recoverresolution_clicked();

    void on_chooseunit_currentTextChanged(const QString &arg1);

    void on_changedpi_clicked();

    void on_recoverdpi_clicked();

    void on_Tabs_currentChanged(int index);

    void on_hide_clicked();

    void on_recovericon_clicked();

    void on_clockseconds_clicked();

    void on_closeclockseconds_2_clicked();

    void on_thenumberoficons_currentTextChanged(const QString &arg1);

    void on_IconPinSpacing_currentTextChanged(const QString &arg1);

    void on_RemoveX_clicked();

    void on_RecoverX_clicked();

    void on_ImmerseStatusBar_clicked();

    void on_ImmerseNavigationBar_clicked();

    void on_ImmerseAll_clicked();

    void on_RecoverAll_clicked();

    void on_changetemperat_clicked();

    void on_changelevel_clicked();

    void on_Non_ChargingSwitch_clicked();

    void on_Wireless_ChargingSwitch_clicked();

    void on_USB_ChargingSwitch_clicked();

    void on_AC_ChargingSwitch_clicked();

    void on_ResetBatteryInfoAndStatus_clicked();

    void on_comboBox_currentTextChanged(const QString &arg1);

    void on_StartBatteryCounting_clicked();

    void on_FontScaling_valueChanged(int value);

    void on_FontScaling_sliderReleased();

    void on_WindowAnimation_valueChanged(int value);

    void on_WindowAnimation_sliderReleased();

    void on_TransitionAnimations_valueChanged(int value);

    void on_TransitionAnimations_sliderReleased();

    void on_AnimatorDuration_valueChanged(int value);

    void on_AnimatorDuration_sliderReleased();

    void on_ChooseTimeUnit_currentTextChanged(const QString &arg1);

    void on_ScreenOff_textEdited(const QString &arg1);

    void on_ChangeScreenOffTime_clicked();

    void on_InputADBCommand_returnPressed();

    void on_UnlockBootloader_clicked();

    void on_LockBootloader_clicked();

    void on_FlashBoot_clicked();

    void on_FlashVbmeta_clicked();

    void on_FlashRecovery_clicked();

    void on_SideloadOTA_clicked();

    void on_FastbootReboot_clicked();

    void on_rebootfastboot_2_clicked();

    void on_rebootrecovery_2_clicked();

    void on_reboot_2_clicked();

    void on_poweroff_2_clicked();

    void on_adbreboot9008_2_clicked();

    void on_fastbootreboot9008_2_clicked();

    void on_TemporaryIMG_clicked();

    void on_FastbootRebootRecovery_clicked();

    void on_SetActivea_clicked();

    void on_SetActiveb_clicked();

    void on_ChoosePartition_currentTextChanged(const QString &arg1);

    void on_ErasePartition_clicked();

    void on_FormatPartition_clicked();

    void on_FlashPartition_clicked();

    void on_BilibiliLink_clicked();

    void on_CoolapkLink_clicked();

    void on_FeedbackProblems_clicked();

    void downloadFile();

    void httpFinished();

    void httpReadyRead();

    //void slotAuthenticationRequired(QNetworkReply *, QAuthenticator *authenticator);    //网络验证授权，暂时用不到

#if QT_CONFIG(ssl)
    void sslErrors(const QList<QSslError> &errors);
#endif

    void on_DownloadOldVersionSoftware_clicked();

private:
    Ui::Widget *ui;
    std::unique_ptr<QFile> openFileForWrite(const QString &fileName);
    QUrl url;
    QNetworkAccessManager qnam;
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply;
    std::unique_ptr<QFile> file;
    bool httpRequestAborted = false;
    bool a = false;
};

#endif // WIDGET_H
