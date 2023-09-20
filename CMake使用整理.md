# CMake使用整理

## CMake简介

​	CMake是一种实现自动化编译的开源的、跨平台的配置工具，通过读取脚本文件CMakeLists.txt来构建编译系统，通常用于配置和编译C/C++程序。

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

在造轮子时，需要将源文件模块制作成二进制的库文件发布给使用者，需要用到add_library函数将指定目录中的源文件生成静态/动态库文件，在发布时，需要传输的有生成的库文件和相应的头文件。

LIBRARY_OUTPUT_PATH宏的值用于指定生成的库文件的位置，若指定路径并不存在，则在生成时会先将指定路径的目录创建出来，再生成文件。

```cmake
set (LIBRARY_OUTPUT_PATH ../lib)
add_library(calcu  SHARED ${SRC})
```

对于需要调用的静态库文件，应使用link_libraries进行链接，传入库名作为参数。当指定库并非系统库时，需要指定链接库的搜寻路径，使用link_directories进行搜寻，传入路径作为参数。

```cmake
link_libraries(calcu)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib1)
```

这样执行编译时，经过静态库的链接和搜寻到的源文件一同被生成为可执行程序（静态库会被打包到可执行程序中）。

对于需要调用的动态库文件，应使用target_link_libraries进行连接。传入库文件名、权限（默认为public）

动态库的链接一般写在可执行程序**生成之后**，指定连接寻找的库文件目录要在可执行程序**生成之前**。

```cmake
target_link_libraries(A B C)
target_link_libraries(D A)
target_link_libraries(app cacu)
```

动态库的链接具有权限传递性，即若库a公有链接库b，库b（未指定）链接库c，那么库a可调用库c的方法。若库b私有链接库c。则只有库b能使用库c的方法。

静态库和动态库的最大区别在于：静态库在程序的链接阶段被打包进可执行程序中，程序执行时会自动加载静态库；而动态库则在程序执行时进行加载，且多个程序加载同一动态库时，共享有同一物理内存。

每个进程加载动态库时，会开辟一块动态库加载区域内存，基于动态库申请的内存都映射于此。

如果需要在编译时进行调试的控制，需要使用message函数进行终端输出。传入消息级别和消息内容，cmake执行到该语句时会根据控制信息做出不同输出。

如果需要在项目编码阶段进行代码段的可用性设置，用于管理调试性代码，可通过在CMake文件中配置自定义宏作为标识，在项目源码中添加如下代码实现该功能：

```c++
#ifdef DEBUGTOW
	std::cout<<"This is test to success."<<std::endl;
#endif
```

CMake中对宏的自定义：

```cmake
add_definitions(-DDEBUGTOW)
```

如果工程项目较大，处于软件工程的考虑，一般会把项目源文件按模块划分至不同文件内进行存放，这时候需要在各源文件所在文件内各设置一个CMakeLists.txt进行调控，再在总文件中设置一个CMakeLists.txt进行宏观控制，让CMake文件形成树的结构。使用add_subdirectory可为父节点CMakeLists.txt添加指定子节点CMakeLists.txt的目录，在父节点中定义的全局变量在子节点中可直接使用。

一般情况下，父节点CMakeLists.txt的需要完成的操作有：指定构建版本、使用C++版本、指定总项目名、指定库文件生成目录、库文件名、指定可执行文件生成目录、可执行程序名、指定头文件搜索目录、添加子节点目录（一句添加一个）等。

子节点CMakeLists.txt需要完成的操作有：指定构建版本、搜索源文件、指定头文件、生成可执行文件或库。

静态库链接静态库后，在进行生成时会包含已链接的静态库内容，所以在生成可执行文件时只需要包含最大的互斥的静态库即可。

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

    除此之外，也可以完成字符串拼接功能。

    ```cmake
    set([VAR] [VALUE1] [VALUE2] [...])
    
    set(LIBRARY_OUTPUT_PATH ../files)
    set(SRC add.cpp main.cpp)
    set(CMAKE_CXX_STANDARD 11)
    
    set(SRC ${SRC} ${SRC1} "HELLO")
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

    设置引入的头文件搜索列表，指定源文件的头文件的路径。

    ```cmake
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
    ```

* add_library()

    根据控制变量符选择性将文件列表集合生成动态或静态库文件。

    生成的库文件名格式为lib+[Name]+.so/dll（动态库）或lib+[Name]+.a/lib（静态库）。

    STATIC表示指定为静态库，SHARED表示为动态库（共享库）。

    ```cmake
    add_library([Name] [STATIC/SHARED] [VAR])
    
    add_library(calcu  SHARED ${SRC})
    ```

* link_libraries()

    指定链接的静态库文件列表，传入的参数为全名或库文件的名字Name（全名称为lib+[Name].a）。

    ```cmake
    link_libraries([<static lib1> <static lib2> <...>])
    
    link_libraries(calcu)
    ```

* link_directories()

    指定链接的库（静态、动态）的搜寻路径。

    ```cmake
    link_directories([DIR])
        
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib1)
    ```

* target_link_libraries()

    指定连接加载的动态库文件名，传入参数为动态库文件名Target、[权限]+[Name]（全名为lib+[Name]+.so）。

    Target代表需要连接动态库的文件，可以是一个源文件，也可以是一个动态库文件或可执行文件。

    ```cmake
    target_link_libraries([Target] <[PRIVATE|PUBLIC|INTERFACE NAME] [PRIVATE|PUBLIC|INTERFACE NAME] [...]>)
    
    target_link_libraries(app cacu)    
    ```

* message()

    执行CMake至此语句时打印出一条消息。

    消息级别默认为重要消息，STATUS为非重要消息，WARNING为CMake警告，SEND_ERROR为CMake错误但会跳过生成步骤继续执行CMake，FATAL_ERROR为CMake错误会种植所有处理过程。

    ```cmake
    message(STATUS "0===XX==XX=XXX=X")
    message(STATUS "1===XX==XX=XXX=X")
    message(SEND_ERROR "2===XX==XX=XXX=X")
    message(FATAL_ERROR "3===XX==XX=XXX=X")    
    ```

* list()

    list函数通常用于字符串处理，生成一个变量，存储的是字符串列表。通过message输出时，会去掉表节点字符串之间存在的“;”，打印出无间隔的串。

    APPEND表示字符串拼接，REMOVE_ITEM表示字符串移除，

    ```cmake
    list([APPEND|REMOVE_ITEM] [VAR] [<list1> <list2> <list3>])
        
    list(APPEND tmp1 " this is Aura ")
    ```

    LENGTH表示求字符串长度，<OUTPUT>表示生成的长度作为一个新的变量（字符串）。

    ```cmake
    list([LENGTH] [VAR] [<OUTPUT>])
    ```

    GET表示获取指定索引的元素，<INDEX>表示索引。<INDEX>为正则从头开始计数，为负则从末尾向前计数，无论正负，绝对值不可大于VAR中的容量。

    ```cmake
    list([GET] [VAR] [<INDEX>])
    ```

    JOIN表示将列表中元素通过**指定字符**进行互相连接后存入新的变量中。

    ```cmake
    list([JOIN] [VAR] [VALUE] [OUTPUT])
    ```

* add_definitions()

    用于添加自定义宏，在-D后直接添加宏名称，编译即可使用。

    ```cmake
    add_definitions([-D+[NAME]] [-D+[NAME]] [...])
    ```

    

* 



## 宏变量介绍

* PROJECT_SOURCE_DIR  [执行cmake命令时选择的路径]
* CMAKE_CURRENT_SOURCE_DIR  [当前的CMakeLists.txt文件所在的路径]
* CMAKE_CXX_STANDARD   [C++的版本]
* LIBRARY_OUTPUT_PATH [生成库文件的输出位置]
* EXECUTABLE_OUTPUT_PATH [生成的可执行文件的位置]
* 