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
#include <QProcess>
#include <QByteArray>
#include <QDebug>

#ifdef WIN32
#define p "\\"
#else
#define p "/"
#endif

QString folder="", pastadir;
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
    {
        pastadir=folder+p+"3ds"+p+"PastaCFW"+p;
        config.setFileName(pastadir+"system.txt");
    }
    else
    {
        pasta.setFileName(folder+p+"PastaCFW"+p+"PastaCFW.3dsx");

        if(pasta.exists())
        {
            pastadir=folder+p+"PastaCFW"+p;
            config.setFileName(pastadir+"system.txt");
        }
        else
        {
            pasta.setFileName(folder+p+"PastaCFW.3dsx");

            if(pasta.exists())
            {
                pastadir=folder+p;
                config.setFileName(pastadir+"system.txt");
            }
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

    if(cfg[1]=='2')
        ui->noAB->setChecked(1);
    if(cfg[2]=='2')
        ui->firmLaunch->setChecked(1);
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
            apply(1, '2');
        else
            apply(1, '1');
    }
}

void MainWindow::on_changepath_clicked()
{
    detectSD();
}

void MainWindow::on_firmLaunch_toggled(bool checked)
{
    if(writable)
    {
        if(checked)
            apply(2, '2');
        else
            apply(2, '0');
    }
}

void MainWindow::on_installfl_clicked()
{
#ifdef WIN32
    QMessageBox::Abort(this, "", "Feature not supported on Windows.");
    return;
#endif

    QString file_url[4] = {
        "http://albertosonic.bplaced.net/pasta/O3DS_file1.bin",
        "http://albertosonic.bplaced.net/pasta/O3DS_file2.bin",
        "http://nus.cdn.c.shop.nintendowifi.net/ccs/download/0004013800000002/00000049",
        "http://nus.cdn.c.shop.nintendowifi.net/ccs/download/0004013800000002/cetk" };
    if(QMessageBox::question(this, "FIRMLAUNCH Install", "Select your Nintendo 3DS model. Make sure updates are not blocked.", "Old 3DS", "New 3DS"))
    {
        file_url[0] = "http://albertosonic.bplaced.net/pasta/N3DS_file1.bin";
        file_url[1] = "http://albertosonic.bplaced.net/pasta/N3DS_file2.bin";
        file_url[2] = "http://nus.cdn.c.shop.nintendowifi.net/ccs/download/0004013820000002/0000000f";
        file_url[3] = "http://nus.cdn.c.shop.nintendowifi.net/ccs/download/0004013820000002/cetk";
    }
    QProcess process;
    process.start("rm \""+pastadir+p+"file3_dec.bin\" \""+pastadir+p+"firm.bin\"");
    while(process.waitForFinished(-1)){}
    for(int i=0; i<4; i++)
    {
        process.start("rm \""+pastadir+p+"file"+QString::number(i+1)+".bin\"");
        while(process.waitForFinished(-1)){}
#ifdef __APPLE__
        process.start("curl -o \""+pastadir+p+"file"+QString::number(i+1)+".bin\" -O \""+file_url[i]+"\"");
#else
        process.start("wget -O \""+pastadir+p+"file"+QString::number(i+1)+".bin\" \""+file_url[i]+"\"");
#endif
        while(process.waitForFinished(-1)){}
    }
    QFile file1(pastadir+p+"file1.bin");
    QFile file2(pastadir+p+"file2.bin");
    QFile file3(pastadir+p+"file3.bin");
    QFile file4(pastadir+p+"file4.bin");

    QByteArray file4_array;
    file4.open(QIODevice::ReadOnly);
    file4.seek(0x1BF);
    file4_array = file4.read(0x10);
    file4.close();

    QByteArray file1_array;
    file1.open(QIODevice::ReadOnly);
    file1.seek(0);
    file1_array = file1.read(0x10);
    file1.close();

    QByteArray key;
    for(int i=0; i<0x10; i++)
        key[i] = file4_array[i]^file1_array[i];

    QByteArray file3_array;
    file3.open(QIODevice::ReadOnly);
    file3.seek(0);
    file3_array = file3.read(0x1C1);
    file3.close();

    process.start("openssl enc -aes-128-cbc -d -in \""+pastadir+p+"file3.bin\" -out \""+pastadir+p+"file3_dec.bin\" -K "+key.toHex()+" -nosalt -iv 0000000000000000");
    while(process.waitForFinished(-1)){}
    QFile file3_dec(pastadir+p+"file3_dec.bin");
    file3_dec.open(QIODevice::ReadOnly);
    QByteArray file3_data = file3_dec.readAll();
    file3_dec.close();

    bool ok;

    QByteArray EXEFS_offset_byte;
    EXEFS_offset_byte.insert(0, file3_data[0x1A3]);
    EXEFS_offset_byte.insert(1, file3_data[0x1A2]);
    EXEFS_offset_byte.insert(2, file3_data[0x1A1]);
    EXEFS_offset_byte.insert(3, file3_data[0x1A0]);
    int EXEFS_offset = EXEFS_offset_byte.toHex().toInt(&ok, 16);
    EXEFS_offset *= 0x200;

    QByteArray EXEFS_size_byte;
    EXEFS_size_byte.append(file3_data[0x1A7]);
    EXEFS_size_byte.append(file3_data[0x1A6]);
    EXEFS_size_byte.append(file3_data[0x1A5]);
    EXEFS_size_byte.append(file3_data[0x1A4]);
    int EXEFS_size = EXEFS_size_byte.toHex().toInt(&ok, 16);
    EXEFS_size *= 0x200;

    file2.open(QIODevice::ReadOnly);
    QByteArray file2_array = file2.read(EXEFS_size);
    file2.close();

    QByteArray firm, final;

    for(int i=0; i<EXEFS_size-0x10; i++)
    {
        firm[i] = file2_array[i]^file3_data[EXEFS_offset+i];
    }

    QFile firmfile(pastadir+p+"firm.bin");
    firmfile.open(QIODevice::WriteOnly);
    for(int i=0; i<EXEFS_size-0x200; i++)
        final[i] = firm[0x200+i];
    firmfile.write(final);
    firmfile.close();

    process.start("rm \""+pastadir+p+"file3_dec.bin\"");
    while(process.waitForFinished(-1)){}
    for(int i=0; i<4; i++)
    {
        process.start("rm \""+pastadir+p+"file"+QString::number(i+1)+".bin\"");
        while(process.waitForFinished(-1)){}
    }

    if((firmfile.size()==962560||firmfile.size()==987136))
        QMessageBox::information(this, "", "firm.bin generated.");
    else
        QMessageBox::warning(this, "", "An error has occurred during the generation of firm.bin.");
}
