# QJsonmodel
QJsonModel is a C++ tree model class for Qt5 which can load json Data. 
If you want to use it for your application, just copy qjsonmodel.h/cpp and qjsonitem.h/cpp into your project. 
QJsonModel is under MIT license

![QJsonModel](https://raw.githubusercontent.com/dridk/QJsonmodel/master/screen.png)



# Usage 

    QJsonModel * model = new QJsonModel;
    QTreeView * view = new QTreeView;
    view->setModel(model);
    model->load("example.json")
