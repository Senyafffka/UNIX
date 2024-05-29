#pragma once
#include "connecteth.h"

#include <QWidget>
#include <QProcess>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void btUseVPN_clicked();
    void btChooseFile_clicked();
    void btSave_clicked();
    void btApply_clicked();
    void btScanWifiNetworks_clicked();
    void btConnect_clicked();
    void chboxUseDefaultPortSSH_clicked();
    void chboxUseDefaultPortRDP_clicked();
    void chboxUseDomainSSH_clicked();

private:
    Ui::MainWindow *ui;
    QProcess* proc;
    QVector<connectETH*> vecEth;
};

enum class protocol
{
    RDP = 0,
    SSH = 1
};
