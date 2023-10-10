# QML(Quick开发)笔记

QML是标记式语言，用于制作声明式UI。

Qt 的QML模块：为QML应用程序提供基础的开发框架。

Qt 的Quick模块，提供可视化组件、模型视图支持、动画框架等用于构建用户界面的功能，是Qt6中用户界面技术的总称，是QML、JavaScript、Qt C++技术的集合。在文档描述中，称是QML的一个标准库。

## 测试原型的开发

step1：创建Qt Quick UI工程 （非代码式编辑）

Qt -- OtherProject -- Qt Quick UI Prototype

在qml中，会存在import QtQuick



①通过import 导入模块

②树形架构，以根节点作为主窗体

③元素的声明为Type{}  属性的声明为 name:value

④QML文档中任意元素可通过其id进行访问，id在一个文档中全局唯一。

⑤有嵌套的特化处理，比如parent关键字访问父节点。



## 属性

```c++
name: value
```

属性是键值对，value有类型。

* 可进行信号交互

    ```c++
    onHightChanged : console.log('height',height);
    //在相应的C++类中定义信号，或在元素内声明信号
    ```

* 可进行焦点切换

    ```c++
    keyNavigation.tab : ...    
    focus : true;
    color: focus? 'red' : 'black';
    ```

* 可进行动态绑定

    使用 ： 进行动态绑定

    ```c++
    text : "this time:" + anotherTimes;
    ```

    动态绑定可通过一次性赋值打破，对属性的修改是覆盖式修改。（类似stylesheet）

* 可通过property 自定义属性

    ```c++
    property int times:24      //自定义属性
    property alias otherTimes:thisLabel.times;   //为某一已声明的属性起别名
    
    ```

* 通过JavaScript的function进行脚本控制，通过qt的信号机制进行通信。

    注意区分值传递和引用传递的区别。



##元素

* Item

    Item是所有视觉元素的基础元素，内含Text、Layout、Flow、MouseArea、shape、Image、Focus等属性（小写）。

* Rectangle

    继承自Item，新增填充颜色、边框、设置圆角等。

    ```c++
    gradient:Gradient{
        GradientStop{position:0.0; color:'red';}
        GradientStop{position:1.0; color:Qt.rgba(Math.radom(),Math.radom(),Math.radom());}
    }
    
    //渐变
    //颜色需要进行比较时使用颜色代码进行字符串比较【小写！】
    
    ```

* Text

    文本元素，有text（字符串处理的属性）、elid（省略，配合width框定文本显示区域）、horizontalAlignment和verticalAlignment对齐属性、style和styleColor渲染文本样式、wrapMode自动换行

    ```c++
    elid:TextElidMiddle   //省略中间
    wrapMode:Text.wordWrap //以文本格式自动换行
        	//（AnyWhere） 以通用格式自动换行
    ```

* Image

    以各种格式显示图像，包含sourec（传入url）、fillMod（控制填充行为）

    ```c++
    fillMod:
    		//ImageStretch  默认，将图像缩放至合适Image元素（拉伸缩放）
    		//ImagePreserveAspectFit 均匀缩放，不裁剪填充 （按比例）
    		//ImagePreserveAspectCrop 均匀缩放，裁剪填充 （按比例）
    		//ImageTile 水平重复填充
    		//ImageTileHorizon  or ImageTileVertical 拉伸+平铺
    		//ImagePad 不转换
    ```

* MouseArea

    不可见矩形区域元素，用于获取鼠标事件。

    一般内置在某个元素内，通过parent等进行控制区域事件的反馈。

    onclicked  点击信号接收

* 定位器【Row Column Grid Flow】

    使用时相当于一个容器。

    Grid中有行和列的概念。（rows 和 columns 用于排列）

    通常用锚点anchors进行容器的位置设置。

    Flow相当于HTML中的flow。

* Repeter

    重复器，通过model指定元素的重复个数，配合容器进行使用（在容器内填充重复元素。）

    通过Positioner.index获取当前重复元素的标号。

    



## 组件

组件指可重用的元素，QML提供了创建组件的不同方法，最简单的是基于文件引入工程的组件创建。

Step1：在文件中放置QML元素并未该文件提供qml名，如Buttom.qml，可使用Buttom同名元素。

tips：一般使用一个Item进行层次封装，在不同文件间访问组件的元素，只能访问到根节点开放出的元素和属性。

​			定义的信号在主元素层使用	on[信号名]	进行接收。



## 简单变换

指平移、旋转、缩放等操作，一般直接对属性进行处理，通过信号进行载入。

```c++
onClicked{
    x+=10;
    rotation+=10;
    scale+=0.1;
}
//后绘制的区域通常在最上一层，除非使用z进行设定。
```

## 输入

MouseArea 作为鼠标输入

TextInput作为文本输入

Keys监控键盘事件

KeyNavigation响应键盘事件获取焦点



## 动画

QML中使用插值的方式控制属性的更改。动画指在一定时间内，一系列属性的变化。

常用的动画元素：

```c++
PropertyAnimation: 属性改变的动画
NumberAnimation： qreal 类型改变的动画 （特化处理）
ColorAnimation：颜色改变的动画（特化处理）
RotationAnimation： 旋转改变的动画（特化处理）
```

动画具有属性：

```c++
property: ""    //绑定属性名，表示动态关联的属性，此时动画需要指明id，同时用start()或restart()启动
form:  //从xxx值开始
to:  //到xxx值结束
duration:  //动画持续时间（ms）
running :   //执行状态控制，true表示执行   可通过绑定一个变量进行监控，通过对事件的处理来控制启动。
target: id   //动画元素绑定的对象元素
```

三种触发方式：

```c++
//属性上的动画：元素完全加载后自行启动
PropertyAnimation  on  y
{
    target: test;
    running:true;
    form: test.y;
    to : test.y+100;
    duration: 1000;
}

//属性上的行为：属性更变时进行延迟、动态变动。
Behavior on y{
    PropertyAnimation{
        duration:1000;
    }
}
onClicked:y-=200;
// 当点击信号被接收时，该元素向上移动200像素点，这个移动的行为以动画方式进行，间隔为1000ms。

//独立动画以start()显示启动或调整running绑定的属性的值进行启动。
```

动画分组：并行或顺序

```c++
SequentialAnimation   //顺序执行
ParallelAnimation    //并行执行

    //两个作为容器，可容纳多个动画元素，在外部调用容器id的start函数进行启动。
```

