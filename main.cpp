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

#include <QApplication>
#include <QTreeView>
#include <QFile>
#include <string>
#include "qjsonmodel.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTreeView * view   = new QTreeView;
    QJsonModel * model = new QJsonModel;

    view->setModel(model);

    std::string json = R"({
                       "firstName": "John",
                       "lastName": "Smith",
                       "age": 25,
                       "address":
                       {
                           "streetAddress": "21 2nd Street",
                           "city": "New York",
                           "state": "NY",
                           "postalCode": "10021"
                       },
                       "phoneNumber":
                       [
                           {
                             "type": "home",
                             "number": "212 555-1234"
                           },
                           {
                             "type": "fax",
                             "number": "646 555-4567"
                           }
                       ]
                   })";



    model->loadJson(QByteArray::fromStdString(json));
    view->show();

    return a.exec();
}
