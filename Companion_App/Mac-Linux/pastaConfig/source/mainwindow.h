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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void detectSD();
    void apply(int offset, char state);

    void on_noAB_toggled(bool checked);

    void on_changepath_clicked();

    void on_firmLaunch_toggled(bool checked);

    void on_installfl_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
