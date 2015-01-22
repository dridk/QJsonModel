#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>
#include "jsonitem.h"
#include "qjsonmodel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    QJsonModel * model = new QJsonModel;

    ui->treeView->setModel(model);



}

MainWindow::~MainWindow()
{
    delete ui;
}
