#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QRadioButton>
#include <QNetworkInterface>


MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int i = 0;
    for(const QNetworkInterface& eth : QNetworkInterface::allInterfaces())
    {
        QString interfaceName = eth.humanReadableName();
        if (interfaceName == "lo" || interfaceName.at(0) == 'w')
            continue;

        connectETH* widgetEth = new connectETH(this);
        widgetEth->setEthName(interfaceName);
        widgetEth->setFixedSize(361, 261);

        ui->tabConnectionType->addTab(widgetEth, interfaceName);
        vecEth.push_back(widgetEth);
        ++i;
    }

    proc = new QProcess(this);

    connect(ui->_btCancel, &QPushButton::clicked, QApplication::instance(), &QCoreApplication::quit);
    connect(ui->_btChooseFile, &QPushButton::clicked, this, &MainWindow::btChooseFile_clicked);
    connect(ui->_btSave, &QPushButton::clicked, this, &MainWindow::btSave_clicked);
    connect(ui->_btApply, &QPushButton::clicked, this, &MainWindow::btApply_clicked);
    connect(ui->_btUseVPN, &QCheckBox::clicked, this, &MainWindow::btUseVPN_clicked);
    connect(ui->_btScanWifiNetworks, &QPushButton::clicked, this, &MainWindow::btScanWifiNetworks_clicked);
    connect(ui->_chboxUseDefaultPortRDP, &QCheckBox::clicked, this, &MainWindow::chboxUseDefaultPortRDP_clicked);
    connect(ui->_chboxUseDefaultPortSSH, &QCheckBox::clicked, this, &MainWindow::chboxUseDefaultPortSSH_clicked);
    connect(ui->_btConnect, &QPushButton::clicked, this, &MainWindow::btConnect_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
    if (proc)
    {
        proc->close();
        proc->waitForFinished();
    }
}


void MainWindow::btSave_clicked()
{
    QString tabName = ui->tabProtocolForConnection->tabText(ui->tabProtocolForConnection->currentIndex());
    if (tabName == "RDP")
    {
        if (ui->_edUsernameRDP->text().isEmpty() || ui->_edPasswordRDP->text().isEmpty() || ui->_edIpAddressRDP->text().isEmpty())
        {
            QMessageBox::warning(0, "Error", "Все поля должны быть заполнены.");
            return;
        }

        QStringList arguments;
        if (!ui->_btUseVPN->isChecked())
        {
            QString command_parameters = QString("VPN_USE=%1\n"
                                                 "RDP_USERNAME=%2\n"
                                                 "RDP_PASSWORD=%3\n"
                                                 "RDP_IP=%4\n"
                                                 "RDP_PORT=%5\n"
                                                 ).arg("no",
                                                  ui->_edUsernameRDP->text(),
                                                  ui->_edPasswordRDP->text(),
                                                  ui->_edIpAddressRDP->text(),
                                                  ui->_edPortRDP->text()
                                                  );

            QString command = QString("echo -n > %1;"
                                      "echo \"" + command_parameters + "\" >> %1"
                                      ).arg("/home/les/rdp.sh");

            arguments << "-c" << command;
        }
        else
        {
            if (ui->_edFilePath->text().isEmpty())
            {
                QMessageBox::warning(0, "Error", "Выберите файл с настройками VPN.");
                return;
            }
            QString command_parameters = QString("VPN_USE=%1\n"
                                                 "VPN_SETTINGS=%2\n"
                                                 "RDP_USERNAME=%3\n"
                                                 "RDP_PASSWORD=%4\n"
                                                 "RDP_IP=%5\n"
                                                 "RDP_PORT=%6\n"
                                                 ).arg("yes",
                                                  ui->_edFilePath->text(),
                                                  ui->_edUsernameRDP->text(),
                                                  ui->_edPasswordRDP->text(),
                                                  ui->_edIpAddressRDP->text(),
                                                  ui->_edPortRDP->text()
                                                  );

            QString command = QString("echo -n > %1;"
                                      "echo \"" + command_parameters + "\" >> %1"
                                      ).arg("/home/les/rdp.sh");
            arguments << "-c" << command;
        }
        proc->start("/bin/sh", arguments);
        proc->waitForFinished();
    }
    else if (tabName == "SSH")
    {
        if (ui->_edUsernameSSH->text().isEmpty() || ui->_edIpAddressSSH->text().isEmpty())
        {
            QMessageBox::warning(0, "Error", "Все поля должны быть заполнены.");
            return;
        }

        QStringList arguments;
        if (!ui->_btUseVPN->isChecked())
        {
            QString command_parameters = QString("VPN_USE=%1\n"
                                                 "SSH_USERNAME=%2\n"
                                                 "SSH_IP=%3\n"
                                                 "SSH_PORT=%4\n"
                                                 ).arg("no",
                                                       ui->_edUsernameSSH->text(),
                                                       ui->_edIpAddressSSH->text(),
                                                       ui->_edPortSSH->text()
                                                      );

            if (ui->_chboxUseDomainSSH->isChecked())
            {
                if (ui->_edDomainSSH->text().isEmpty())
                {
                    QMessageBox::warning(0, "Error", "Введите домен.");
                    return;
                }
                command_parameters += QString("SSH_USE_DOMAIN=%1\n"
                                              "SSH_DOMAIN=%2\n"
                                              ).arg("yes",
                                                    ui->_edDomainSSH->text()
                                                    );
            }
            else
                command_parameters += "SSH_USE_DOMAIN=no\n";

            QString command = QString("echo -n > %1;"
                                      "echo \"" + command_parameters + "\" >> %1"
                                      ).arg("/home/les/rdp.sh");

            arguments << "-c" << command;
        }
        else
        {
            if (ui->_edFilePath->text().isEmpty())
            {
                QMessageBox::warning(0, "Error", "Выберите файл с настройками VPN.");
                return;
            }
            QString command_parameters = QString("VPN_USE=%1\n"
                                                 "SSH_SETTINGS=%2\n"
                                                 "SSH_USERNAME=%3\n"
                                                 "SSH_IP=%4\n"
                                                 "SSH_PORT=%5\n"
                                                 ).arg("yes",
                                                  ui->_edFilePath->text(),
                                                  ui->_edUsernameSSH->text(),
                                                  ui->_edIpAddressSSH->text(),
                                                  ui->_edPortSSH->text()
                                                  );

            if (ui->_chboxUseDomainSSH->isChecked())
            {
                if (ui->_edDomainSSH->text().isEmpty())
                {
                    QMessageBox::warning(0, "Error", "Введите домен.");
                    return;
                }
                command_parameters += QString("SSH_USE_DOMAIN=%1\n"
                                              "SSH_DOMAIN=%2\n"
                                              ).arg("yes",
                                                    ui->_edDomainSSH->text()
                                                    );
            }
            else
                command_parameters += "SSH_USE_DOMAIN=no\n";

            QString command = QString("echo -n > %1;"
                                      "echo \"" + command_parameters + "\" >> %1"
                                      ).arg("/home/les/rdp.sh");

            arguments << "-c" << command;
        }
        proc->start("/bin/sh", arguments);
        proc->waitForFinished();
    }

    QApplication::instance()->quit();
}


void MainWindow::btApply_clicked()
{
    QString tabName = ui->tabProtocolForConnection->tabText(ui->tabProtocolForConnection->currentIndex());
    if (tabName == "RDP")
    {
        if (ui->_edUsernameRDP->text().isEmpty() || ui->_edPasswordRDP->text().isEmpty() || ui->_edIpAddressRDP->text().isEmpty())
        {
            QMessageBox::warning(0, "Error", "Все поля должны быть заполнены.");
            return;
        }

        QStringList arguments;
        if (!ui->_btUseVPN->isChecked())
        {
            QString command_parameters = QString("VPN_USE=%1\n"
                                                 "RDP_USERNAME=%2\n"
                                                 "RDP_PASSWORD=%3\n"
                                                 "RDP_IP=%4\n"
                                                 "RDP_PORT=%5\n"
                                                 ).arg("no",
                                                  ui->_edUsernameRDP->text(),
                                                  ui->_edPasswordRDP->text(),
                                                  ui->_edIpAddressRDP->text(),
                                                  ui->_edPortRDP->text()
                                                  );

            QString command = QString("echo -n > %1;"
                                      "echo \"" + command_parameters + "\" >> %1"
                                      ).arg("/home/les/rdp.sh");

            arguments << "-c" << command;
        }
        else
        {
            if (ui->_edFilePath->text().isEmpty())
            {
                QMessageBox::warning(0, "Error", "Выберите файл с настройками VPN.");
                return;
            }
            QString command_parameters = QString("VPN_USE=%1\n"
                                                 "VPN_SETTINGS=%2\n"
                                                 "RDP_USERNAME=%3\n"
                                                 "RDP_PASSWORD=%4\n"
                                                 "RDP_IP=%5\n"
                                                 "RDP_PORT=%6\n"
                                                 ).arg("yes",
                                                  ui->_edFilePath->text(),
                                                  ui->_edUsernameRDP->text(),
                                                  ui->_edPasswordRDP->text(),
                                                  ui->_edIpAddressRDP->text(),
                                                  ui->_edPortRDP->text()
                                                  );

            QString command = QString("echo -n > %1;"
                                      "echo \"" + command_parameters + "\" >> %1"
                                      ).arg("/home/les/rdp.sh");
            arguments << "-c" << command;
        }
        proc->start("/bin/sh", arguments);
        proc->waitForFinished();
    }
    else if (tabName == "SSH")
    {
        if (ui->_edUsernameSSH->text().isEmpty() || ui->_edIpAddressSSH->text().isEmpty())
        {
            QMessageBox::warning(0, "Error", "Все поля должны быть заполнены.");
            return;
        }

        QStringList arguments;
        if (!ui->_btUseVPN->isChecked())
        {
            QString command_parameters = QString("VPN_USE=%1\n"
                                                 "SSH_USERNAME=%2\n"
                                                 "SSH_IP=%3\n"
                                                 "SSH_PORT=%4\n"
                                                 ).arg("no",
                                                       ui->_edUsernameSSH->text(),
                                                       ui->_edIpAddressSSH->text(),
                                                       ui->_edPortSSH->text()
                                                      );

            if (ui->_chboxUseDomainSSH->isChecked())
            {
                if (ui->_edDomainSSH->text().isEmpty())
                {
                    QMessageBox::warning(0, "Error", "Введите домен.");
                    return;
                }
                command_parameters += QString("SSH_USE_DOMAIN=%1\n"
                                              "SSH_DOMAIN=%2\n"
                                              ).arg("yes",
                                                    ui->_edDomainSSH->text()
                                                    );
            }
            else
                command_parameters += "SSH_USE_DOMAIN=no\n";

            QString command = QString("echo -n > %1;"
                                      "echo \"" + command_parameters + "\" >> %1"
                                      ).arg("/home/les/rdp.sh");

            arguments << "-c" << command;
        }
        else
        {
            if (ui->_edFilePath->text().isEmpty())
            {
                QMessageBox::warning(0, "Error", "Выберите файл с настройками VPN.");
                return;
            }
            QString command_parameters = QString("VPN_USE=%1\n"
                                                 "SSH_SETTINGS=%2\n"
                                                 "SSH_USERNAME=%3\n"
                                                 "SSH_IP=%4\n"
                                                 "SSH_PORT=%5\n"
                                                 ).arg("yes",
                                                  ui->_edFilePath->text(),
                                                  ui->_edUsernameSSH->text(),
                                                  ui->_edIpAddressSSH->text(),
                                                  ui->_edPortSSH->text()
                                                  );

            if (ui->_chboxUseDomainSSH->isChecked())
            {
                if (ui->_edDomainSSH->text().isEmpty())
                {
                    QMessageBox::warning(0, "Error", "Введите домен.");
                    return;
                }
                command_parameters += QString("SSH_USE_DOMAIN=%1\n"
                                              "SSH_DOMAIN=%2\n"
                                              ).arg("yes",
                                                    ui->_edDomainSSH->text()
                                                    );
            }
            else
                command_parameters += "SSH_USE_DOMAIN=no\n";

            QString command = QString("echo -n > %1;"
                                      "echo \"" + command_parameters + "\" >> %1"
                                      ).arg("/home/les/rdp.sh");

            arguments << "-c" << command;
        }
        proc->start("/bin/sh", arguments);
        proc->waitForFinished();
    }
}


void MainWindow::btChooseFile_clicked()
{
    QString path = QFileDialog::getOpenFileName(0, QObject::tr("Укажите файл VPN"), QDir::homePath(), QObject::tr("Файл VPN (*.ovpn);;Все файлы (*.*)"));
    ui->_edFilePath->setText(path);
}


void MainWindow::btUseVPN_clicked()
{
    if (ui->_btUseVPN->isChecked())
    {
        ui->_btUseVPN->setText("Выкл");
        ui->_btChooseFile->setEnabled(true);
        ui->_edFilePath->setEnabled(true);
    }
    else
    {
        ui->_btUseVPN->setText("Вкл");
        ui->_btChooseFile->setEnabled(false);
        ui->_edFilePath->setEnabled(false);
    }
}


void MainWindow::btConnect_clicked()
{
    QString tabName = ui->tabConnectionType->tabText(ui->tabConnectionType->currentIndex());

//  Wi-Fi
    if (tabName == "Wi-Fi")
    {
        if (!ui->_tableWifiNetworks->currentItem()->isSelected() || ui->_tableWifiNetworks->currentColumn() == 1)
        {
            QMessageBox::warning(0, "Error", "Не выбрано Wi-Fi соединение.");
            return;
        }

        int col = ui->_tableWifiNetworks->currentColumn();
        int row = ui->_tableWifiNetworks->currentRow();

        QStringList arguments;
        QString command = QString("nmcli -f CONNECTION,TYPE dev status | grep wifi");
        arguments << "-c" << command;
        proc->start("/bin/sh", arguments);
        proc->waitForFinished();

        QString output = "\n" + proc->readAllStandardOutput();

        int it = 0;
        while (true)
        {
            it = output.indexOf("\n");
            if (it == output.length() - 1 || it == -1)
                break;
            ++it;

            QString name_connect;
            while(output[it] != ' ')
            {
                name_connect += output[it];
                ++it;
            }

            if (name_connect == ui->_tableWifiNetworks->item(row, col)->text())
            {
                ui->_tableWifiNetworks->clearSelection();
                return;
            }

            arguments.clear();
            command = QString("nmcli con down %1").arg(name_connect);
            arguments << "-c" << command;
            proc->start("/bin/sh", arguments);
            proc->waitForFinished();

            output.remove(0, it);
        }

        arguments.clear();
        command = QString("nmcli dev wifi connect %1").arg(ui->_tableWifiNetworks->item(row, col)->text());
        arguments << "-c" << command;
        proc->start("/bin/sh", arguments);
        proc->waitForFinished();

        ui->_tableWifiNetworks->clearSelection();

        return;
    }

//  Ethernet
    QString dirEtcNetworkInterfaces = "/etc/network/interfaces";

    QStringList arguments;
    QString command_parameters = QString("auto lo\n"
                                         "iface lo inet loopback\n\n"
                                         );

    QString command = QString("echo -n > %1;").arg(dirEtcNetworkInterfaces);
    QString ifup_comand = "ifup lo;";

    for (int i = 0; i < vecEth.size(); ++i)
        if (vecEth.at(i)->getBtTurn()->isChecked())
        {
            if(vecEth.at(i)->getRDBAuto()->isChecked())
            {
                command_parameters += QString("auto %1\n"
                                              "iface %1 inet dhcp\n\n"
                                              ).arg(vecEth.at(i)->getEthName());

                ifup_comand += QString("ifup %1;").arg(vecEth.at(i)->getEthName());
            }
            else if(vecEth.at(i)->getRDBManually()->isChecked())
            {
                if (vecEth.at(i)->getGateway()->text().isEmpty() || vecEth.at(i)->getIpAddress()->text().isEmpty() || vecEth.at(i)->getSubnetMask()->text().isEmpty())
                {
                    QMessageBox::warning(0, "Error", "Все поля должны быть заполнены.");
                    return;
                }

                command_parameters += QString("auto %1\n"
                                              "iface %1 inet static\n"
                                              "address %2\n"
                                              "gateway %3\n"
                                              "netmask %4\n\n"
                                              ).arg(vecEth.at(i)->getEthName(),
                                                    vecEth.at(i)->getIpAddress()->text(),
                                                    vecEth.at(i)->getGateway()->text(),
                                                    vecEth.at(i)->getSubnetMask()->text()
                                                    );

                ifup_comand += QString("ifup %1;").arg(vecEth.at(i)->getEthName());
            }
        }
    command = QString("echo \"" + command_parameters + "\" >> %1;" + ifup_comand).arg(dirEtcNetworkInterfaces);

    arguments << "-c" << command;
    proc->start("/bin/sh", arguments);
    proc->waitForFinished();
}


void MainWindow::btScanWifiNetworks_clicked()
{
    ui->_tableWifiNetworks->clearContents();
    ui->_tableWifiNetworks->model()->removeRows(0, ui->_tableWifiNetworks->rowCount());

    QStringList arguments;
    QString command = QString("nmcli -f SSID,SECURITY dev wifi list");

    arguments << "-c" << command;
    proc->start("/bin/sh", arguments);
    proc->waitForFinished();

    QString output = proc->readAllStandardOutput();

    int it = 0, countWiFiNetworks = 0;
    while (true)
    {
        it = output.indexOf("\n");
        if (it == output.length() - 1 || it == -1)
            break;
        ++it;

        QString name_connect;
        while(output[it] != ' ')
        {
            name_connect += output[it];
            ++it;
        }

        while (output[it] == ' ')
            ++it;

        bool secure_connect = false;
        if (output[it] == 'W')
            secure_connect = true;

        ui->_tableWifiNetworks->insertRow(countWiFiNetworks);
        ui->_tableWifiNetworks->setItem(countWiFiNetworks, 0, new QTableWidgetItem(name_connect));

        if (secure_connect)
            ui->_tableWifiNetworks->setItem(countWiFiNetworks, 1, new QTableWidgetItem("Secure"));
        else
            ui->_tableWifiNetworks->setItem(countWiFiNetworks, 1, new QTableWidgetItem("Not secure"));

        ui->_tableWifiNetworks->item(countWiFiNetworks, 0)->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        ui->_tableWifiNetworks->item(countWiFiNetworks, 1)->setFlags(Qt::ItemIsEnabled);

        ++countWiFiNetworks;
        output.remove(0, it);
    }
}


void MainWindow::chboxUseDefaultPortSSH_clicked()
{
    if (!ui->_chboxUseDefaultPortSSH->isChecked())
        ui->_edPortSSH->setEnabled(true);
    else
        ui->_edPortSSH->setEnabled(false);
}


void MainWindow::chboxUseDefaultPortRDP_clicked()
{
    if (!ui->_chboxUseDefaultPortRDP->isChecked())
        ui->_edPortRDP->setEnabled(true);
    else
        ui->_edPortRDP->setEnabled(false);
}


void MainWindow::chboxUseDomainSSH_clicked()
{
    if (ui->_chboxUseDomainSSH->isChecked())
        ui->_edDomainSSH->setEnabled(true);
    else
        ui->_edDomainSSH->setEnabled(false);
}

