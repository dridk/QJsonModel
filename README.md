# QJsonModel
QJsonModel is a JSON tree model class for Qt6/C++17 based on QAbstractItemModel.

QJsonModel was originally written by Sacha Shutz (https://github.com/dridk).

This fork is also released under the MIT License.


![QJsonModel](https://gitea.beniquez.me/sdaveb/QJsonModel/raw/branch/master/screen.png)

## Build Instructions

### Build Tools 
- CMake (version 3.21 or higher)
- C++17-compatible compiler

### Building the Project
1. Clone the repository:
    ```
    git clone <URL>
    ```

2. Navigate to the project directory:
    ```
    cd  elemental-game
    ```
3. Configure your build system:
    ```bash
    cmake -B debug -G Unix Makefiles
    # or
    cmake -B debug -G Ninja   # this is faster and more modern
    ```
4. Invoke your build system
    ```
    cmake --build debug
    ```
### Usage - CMake

You can add this library to your CMake projects using FetchContent() 
or CPM_AddPackage().

Here's how to do it with CPM_AddPackage:

```
COMING SOON
```

### Usage - C++

#### 
```cpp
QJsonModel * model = new QJsonModel;
QTreeView * view = new QTreeView;
view->setModel(model);
model->load("example.json")
```
