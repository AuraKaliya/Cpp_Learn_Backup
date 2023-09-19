# CMake使用整理

## CMake简介

​	CMake是一种高级编译的配置工具，通常用于配置和编译C/C++程序。

##使用

CMake在使用时，需要对CMakeLists.txt文件进行编写，用于配置编译时的具体操作信息。通常，一个能够执行基本编译功能的CMakeLists.txt文件需要具备以下配置信息：指定CMake和代码语言的版本、指定工程名、取得需要进行编译的文件并编译成可执行文件。

```cmake
cmake_minimum_required(VERSION 3.20)
#使用的CMake至少为3.20版本
    
project(test)
#工程名为test    
   
set(SRC add.h add.cpp main.cpp)
#为SRC赋值为文件名列表

set(CMAKE_CXX_STANDARD 11)
#设置构建的C++的版本为 C++11
   
add_executable(app ${SRC})
 #从SRC中取出文件，在执行make时生成名为app的可执行文件 
```

文件列表可用**分号**隔开，也可以仅用**空格**进行分隔。

在编译时，通常调用以下命令：

```
cmake  [CMakeList.txt文件所在路径]
```

cmake执行后，当前目录下会生成Makefile、cmake_install.cmake、CMakeFiles、CMakeCache.txt等文件，这时候调用make命令，可生成相应的文件。

```c++
make
```

对于一个工程项目来说，需要将头文件和源文件**分离**，并为特定模块的源提供单独的存放区域。CMake中提供file函数用于**搜索**文件夹中的文件，include_directories用于配置引入的头文件列表，EXECUTABLE_OUTPUT_PATH宏的值用于指定生成的文件的位置，CMAKE_CURRENT_SOURCE_DIR宏的值用于取得当前CMakeList.txt文件的位置。使用这些语句以更精确化和便捷地控制编译：

```cmake
#aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC)
file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )   
#表示搜索该目录（CMakeLists.txt）下的/src目录的所有.cpp文件生成一个列表存放在SRC中

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
#设置头文件搜索列表为该目录${CMAKE_CURRENT_SOURCE_DIR}/include

set(EXECUTABLE_OUTPUT_PATH  ../files)
#设置 生成的可执行文件的位置  相对于CMakeLists.txt所在的位置
```

## 命令介绍&词典

cmake中相关功能的命令参数的介绍。

* cmake_minimum_required()

    为变量指定最低值，通常用于指定cmake的版本

    ```cmake
    cmake_minimum_required([VAR]  [VALUE])
    
    cmake_minimum_required(VERSION 3.15)
    ```

* project()

    对工程的参数进行配置，分别指定工程名、版本、描述，一般情况下，指定工程名即可。

    ```cmake
    project([PROJECT-NAME],[VERSION],[DESCRIPTION])
    
    project(test)
    ```

* set()

    设置/改变变量的值，通常用于指定配置信息，当有多个变量值时，通常存放结构为“列表”。

    ```cmake
    set([VAR] [VALUE1] [VALUE2] [...])
    
    set(LIBRARY_OUTPUT_PATH ../files)
    set(SRC add.cpp main.cpp)
    set(CMAKE_CXX_STANDARD 11)
    ```

* aux_source_directory()

    查找某个路径下的所有源文件，并向变量中存入一个由文件名构成的列表。

    ```cmake
    aux_source_directory([DIR] [VAR])
    
    aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC)
    ```

* file()

    根据控制变量符指定搜索目标路径的所有符合条件的文件，并向变量中存放这些文件名构成的列表。

    其中，GLOB表示只在指定目录中进行搜索，GLOB_RECURSE表示在指定目录下进行递归地搜索。

    ```cmake
    file([GLOB/GLOB_RECURSE] [VAR] [DIR])
    
    file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )   
    ```

* include_directories()

    设置引入的头文件搜索列表。

    ```cmake
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
    ```

* add_library()

    生成库文件。

* 



## 宏变量介绍

* PROJECT_SOURCE_DIR  [执行cmake命令时选择的路径]
* CMAKE_CURRENT_SOURCE_DIR  [当前的CMakeLists.txt文件所在的路径]
* CMAKE_CXX_STANDARD   [C++的版本]
* LIBRARY_OUTPUT_PATH [生成库文件的输出位置]
* 