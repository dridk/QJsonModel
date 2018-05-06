# QJsonModel
QJsonModel is a json tree model class for Qt5/C++11/Python based on QAbstractItemModel.
QJsonModel is under MIT License. 

![QJsonModel](https://raw.githubusercontent.com/dridk/QJsonmodel/master/screen.png)

## Usage C++

Add `qjsonmodel.cpp` and `qjsonmodel.h` into your project. 

```cpp
QJsonModel * model = new QJsonModel;
QTreeView * view = new QTreeView;
view->setModel(model);
model->load("example.json")
```

## Usage Python

Add `qjsonmodel.py` to your `PYTHONPATH`.

```bash
$ pip install Qt.py
```

```python	
import json
import qjsonmodel

model = QJsonModel()
view = QTreeView()
view.setModel(model)

with open("example.json") as f:
	model.load(json.load(f))
```
