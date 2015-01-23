/***********************************************
    Copyright (C) 2014  Schutz Sacha
    This file is part of QJsonModel (https://github.com/dridk/QJsonmodel).

    QJsonModel is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QJsonModel is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QJsonModel.  If not, see <http://www.gnu.org/licenses/>.

**********************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include "qjsonitem.h"
#include "qjsonmodel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mModel= new QJsonModel;

    ui->treeView->setModel(mModel);
    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(openFile()));
    connect(ui->tabWidget,SIGNAL(tabBarClicked(int)),this,SLOT(tabChanged(int)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile()
{

    QString filename = QFileDialog::getOpenFileName(this,tr("Open Json file"),"",tr("Json file (*.json)"));

    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)){
        ui->textEdit->setPlainText(file.readAll());
        file.close();
        ui->tabWidget->setCurrentIndex(0);
    }



}

void MainWindow::tabChanged(int index)
{

    if (index == 0) {
        if (!mModel->loadJson(ui->textEdit->toPlainText().toUtf8()))
            QMessageBox::warning(this,"error","Cannot load Json Data");
    }


}



