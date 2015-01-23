#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>
#include <QFileDialog>
#include "qjsonitem.h"
#include "qjsonmodel.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mModel= new QJsonModel;

    ui->treeView->setModel(mModel);

    mModel->load("/home/sacha/test.json");

    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(openFile()));


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openFile()
{

    QString filename = QFileDialog::getOpenFileName(this,tr("Open Json file"),"",tr("Json file (*.json)"));
    mModel->load(filename);



}
