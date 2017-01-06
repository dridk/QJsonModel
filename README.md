# QJsonModel
QJsonModel is a json tree model class for Qt5/C++11.
QJsonModel is under MIT License. 

![QJsonModel](https://raw.githubusercontent.com/dridk/QJsonmodel/master/screen.png)

## Installation 
Add QJsonModel.cpp and QJsonModel.h into your project. 

## Usage 

    QJsonModel * model = new QJsonModel;
    QTreeView * view = new QTreeView;
    view->setModel(model);
    model->load("example.json")
