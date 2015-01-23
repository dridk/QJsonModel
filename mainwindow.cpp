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



