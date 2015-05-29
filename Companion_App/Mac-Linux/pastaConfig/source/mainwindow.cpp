/*
 * Copyright © 2015 nastys
 *
 * This file is part of pastaConfig.
 * redTools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * redTools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with pastaConfig.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>

#ifdef WIN32
#define p "\\"
#else
#define p "/"
#endif

QString folder="";
QByteArray cfg="000";
QFile config;
bool writable=0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    detectSD();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::detectSD()
{
#ifdef __APPLE__
    folder=QFileDialog::getExistingDirectory(this, "Open SD card", "/Volumes/", QFileDialog::ShowDirsOnly);
#elif WIN32
    folder=QFileDialog::getExistingDirectory(this, "Open SD card", "::{20D04FE0-3AEA-1069-A2D8-08002B30309D}", QFileDialog::ShowDirsOnly);
#else
    folder=QFileDialog::getExistingDirectory(this, "Open SD card", "/media/"+qgetenv("USER"), QFileDialog::ShowDirsOnly);
#endif

    if(folder=="")
        exit(0);

    QFile pasta(folder+p+"3ds"+p+"PastaCFW"+p+"PastaCFW.3dsx");

    if(pasta.exists())
        config.setFileName(folder+p+"3ds"+p+"PastaCFW"+p+"system.txt");
    else
    {
        pasta.setFileName(folder+p+"PastaCFW"+p+"PastaCFW.3dsx");

        if(pasta.exists())
            config.setFileName(folder+p+"PastaCFW"+p+"system.txt");
        else
        {
            pasta.setFileName(folder+p+"PastaCFW.3dsx");

            if(pasta.exists())
                config.setFileName(folder+p+"system.txt");
            else
            {
                QMessageBox::critical(this, "Error", "Pasta CFW not found.");
                exit(1);
            }
        }
    }

    ui->changepath->setText(folder);

    config.open(QIODevice::ReadWrite);
    if(config.size()<3)
    {
        QMessageBox::warning(this, "Warning", "The configuration file is invalid and will be recreated.");
        config.close();
        config.remove();
        config.open(QIODevice::ReadWrite);
        config.write("000");
    }
    else
        cfg=config.read(3);
    config.close();

    if(cfg[1]=='1')
        ui->noAB->setChecked(1);
    if(cfg[2]=='1')
        ui->dumpMEM->setChecked(1);
    writable=1;

    switch(cfg[0])
    {
    case '0': // none
        ui->fw->setText("None");
        break;
    case '1': // 4.x
        ui->fw->setText("Old 3DS v4.1-4.5");
        break;
    case '2': // 5.0
        ui->fw->setText("Old 3DS v5.0");
        break;
    case '3': // 5.1
        ui->fw->setText("Old 3DS v5.1");
        break;
    case '4': // 6.0
        ui->fw->setText("Old 3DS v6.0");
        break;
    case '5': // 6.x
        ui->fw->setText("Old 3DS v6.1-6.3");
        break;
    case '6': // 7.0-7.1
        ui->fw->setText("Old 3DS v7.0-7.1");
        break;
    case '7': // 7.2
        ui->fw->setText("Old 3DS v7.2");
        break;
    case '8': // 8.x
        ui->fw->setText("Old 3DS v8.0-8.1");
        break;
    case '9': // 9.x
        ui->fw->setText("Old 3DS v9.0-9.2");
        break;
    case 'a': // New 8.x
        ui->fw->setText("New 3DS v8.1");
        break;
    case 'b': // New 9.0
        ui->fw->setText("New 3DS v9.0-9.2");
        break;
    default:
        ui->fw->setText("Unknown");
    }
}

void MainWindow::apply(int offset, char state)
{
    cfg[offset]=state;
    config.open(QIODevice::WriteOnly);
    config.write(cfg);
    config.close();
}

void MainWindow::on_noAB_toggled(bool checked)
{
    if(writable)
    {
        if(checked)
            apply(1, '1');
        else
            apply(1, '0');
    }
}

void MainWindow::on_dumpMEM_toggled(bool checked)
{
    if(writable)
    {
        if(checked)
            apply(2, '1');
        else
            apply(2, '0');
    }
}

void MainWindow::on_changepath_clicked()
{
    detectSD();
}
