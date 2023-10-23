# OpenGL学习心得

## OpenGL概念

OpenGL本身是图形库的API，是一种“规范”，只是允许访问显卡，并不提供代码。

七大功能：建立3D模型、图形变换、颜色模式、光照材质的设置、纹理映射、图像增强功能和位图显示扩展功能、双缓存功能。

规范地描述了绘制2D和3D图形的抽象API，语言无关、平台无关。

被设计为只有输出的，只提供“渲染”的概念性功能。

OpenGL提供的函数接口的设计思路是面向过程的，基于C的。

* OpenGL上下文

    在应用程序调用任何OpenGL指令之前，需要创建一个OpenGL上下文，OpenGL是一个庞大的状态机，上下文存储了OpenGL中的各种状态，是OpenGL执行命令的基础。

    通常有两个类型的函数：**状态设置函数**（State-Changing Function）和**状态应用函数**（State-Using Function）。

    切换上下文会产生巨大的开销，但绘制不同的模块夜会应用到不同的状态管理。

    ①创建不同的上下文	②在不同线程/函数方法中使用	③上下文之间共享缓冲区等资源，只是标记不同

* 帧缓冲区

    OpenGL是图形库API，所有的运算结果都通过图像进行输出，而Buffer相当于画布。

* 附着

    关联“画布”，有三种类型对应三种缓冲区：颜色、深度、模板。

    实际存储图像数据的区域：纹理和渲染缓冲区。

* 顶点数组（Vertex）

    OpenGL的图像都是由图元组成：点、线、三角形。

    开发者可设定函数指针绑定的绘制方法，由内存传入顶点数据或提前在显存中分配一块缓冲区进行预存入。

* 着色器程序（Shader）

    在OpenGL的实际调用绘制函数之前需要指定由Shader编译成的着色器程序。

    常见的着色器程序有：顶点着色器（Vertex）、片段着色器（Fragment）、像素着色器（Pixel）、几何着色器（Geomentry）、曲面细分着色器（Tessellation）。

    ①由顶点着色器**传入**的**顶点数据**进行运算，再进行**图元的装配**，将顶点转换为图元。

    ②进行光栅化，将图元（矢量）转化为栅格化数据。

    ③将栅格化数据传入片段着色器中进行运算，对每个像素数据进行逻辑处理。

    ④最终处理后的像素进入测试阶段，之后和帧缓冲区上的颜色附着的颜色进行混合，在画布上显示。



### GLEW

引入的头文件需要放在GLFW之前。

使用glewInit()需要先完成宏定义GLEW_STATIC（表示引入的是静态库），在项目-属性-C++-预处理中添加。



## Qt中的OpenGL

Qt中已封装的OpenGL方法能平替以下两个模块功能：

* GLFW

    解决操作系统层面的不同：**创建窗口、定义上下文、处理用户输入**。

* GLAD

    使代码可用于不同的OpenGL驱动。

在Qt的封装下，不用关系平台问题。

OpenGL核心是一个C库，支持多语言派生。

* 核心模式

    现代模式、可编程模式。

    在**GPU渲染流水线**：顶点着色器--图元装配--几何着色器--光栅化--片段着色器--测试与混合  中，对**着色器**部分**可编程**。

* OpenGL对象

    一个对象代表是一些选项的集合，内置OpenGL的多个属性状态。而一个Context包含多个对象。

    ```c++
    //创建对象使用“Id”区访问这个对象。
    GLuint objectId=0;
    glGenObject(1,&objectId);
    
    //将对象与上下文GL_WINDOW_TARGET绑定
    glBindObject(GL_WINDOW_TARGET,objectId);
    
    //对上下文绑定的当前对象进行属性/状态的设置
    glSetObjectOption(GL_WINDOW_TARGET,GL_OPTION_WINDOW_WIDTH,800);
    glSetObjectOption(GL_WINDOW_TARGET,GL_OPTION_WINDOW_HEIGHT,600);
    
    //进行解绑
    glBindObject(GL_WINDOW_TARGET,0);
    
    //当重新绑定至对象时，这些“选项”便会再次生效，通过对上下文GL_WINDOW_TARGET的当前绑定对象进行访问查询得知。
    ```

* QOpenGLWidget

    实现GLFW的功能。

    提供三个便捷的虚函数，可以重载以完成典型的OpenGL任务。

    ```c++
         void initializeGL()override;
         void resizeGL(int w, int h)override;
         void paintGL()override;
    
    /*
    paintGL：用于渲染OpenGL场景，当Widget需要更新时进行调用。
    resizeGL：设置OpenGL的视口、投影等，当Widget调整大小和首次显示时调用。
    initializeGL：设置OpenGL的资源和状态，在调用另外俩函数之前需要进行设置。
    
    若要从paintGL以外的位置触发重回，则使用QWidget自带的update。
    若要从其它位置调用标准的OpenGL的API，必须先调用makeCurrent();
    执行的是覆盖式渲染。
    */
    ```

* **Qt6在使用时需要在工程中添加QT+=openglwidgets**

    * QOpenGLFunction_X_X_Core

        提供GLAD功能。

        提供OpenGL x.x版本核心模式的所有功能，是对OpenGL函数的封装。

    *   initializeOpenGLFunctions();

        初始化OpenGL函数，将Qt中的函数指针指向显卡的函数。

        在initializeGL函数中进行调用。

* 标准化设备坐标

    顶点着色器中处理后的顶点（坐标点）应该是标准化设备的坐标，xyz的值在-1.0到1.0之间，以浮点数的格式，落在范围之外的坐标点会被剪裁。

    对应整个窗体的大小，窗体的中心点为(0,0)。

* 着色器应用

    顶点着色器在GPU上创建内存，用于存储顶点的数据。这些数据通过顶点缓冲对象（Vertex Buffer Object  VBO）进行管理，缓冲类型为GL_ARRAY_BUFFER。

    需要配置OpenGL对这些内存的解释器：顶点数组对象（Vertex Array Objects VAO）进行解释，它并不实际存储数据，仅存放这些顶点的定义（一组数据几个属性、数据存放在第几个元素指向的区域内、指针移动步长）,即解析。（内有一张表用于对应显存中的数据结构）
    
* VBO、VAO和EBO

    VBO存储实际数据。

    VAO存储数据定义。

    * EBO存储数据对象的索引。

        对于多点图的绘制，若有重复出现/使用的点，则可以考虑使用源+索引的方式进行数据取得。

        在VAO中绑定设置EBO，根据EBO中的索引从VBO中拿数据。

        ```c++
        unsigned int EBO;
        glGenBuffers(1,&EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
        /*
        1. 创建EBO。
        2. 绑定缓冲区：GL_ELEMENT_ARRAY_BUFFER
        3. 向缓冲区传数据（索引信息）。		这时候VAO会同时进行记录
        4. 索引信息和用索引取的这个方式记录在VAO中。
        */
        
        glDrawElement(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        /*
        从EBO中取索引，依照索引从相应的VBO中取6个数据，绘制成面。
        若此时的VAO没绑定EBO，则最后一个参数填EBO的地址。
        */
        ```

* 填充方式的选择：glPolygonMod

    ```c++
    glPolygonMod(GL_FRONT_AND_BACK,GL_LINE);
    
    //多边形的绘制有两面。默认情况下，顺时针绘制时反面，逆时针绘制是正面。
    //绘制方式有GL_LINE 线框、GL_FILL 填充、GL_POINT 点。
    //通过glFrontFace(GL_CCW/GL_CW)设定当前选取的正面是逆时针(GL_CCW)还是顺时针(GL_CW)。
    ```

* 注意添加对象的回收

    通常放在析构函数中。

    ```c++
    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,VBO);
    glDeleteProgram(shaderProgram);
    
    /*
    由于是在三个虚函数之外调用，代码需要用makeCurrent(); doneCurrent()括住。
    */
    ```

* 一种动态绘制的思路

    在paintGL()、resizeGL()、initializeGL()之外的地方调用绘制需要有一个makeCurrent()、...... 、doneCurrent（）的过程。

    ①在paintGL中进行状态的判定和选择性绘制。

    ②在外部通过接口改变状态+update();

* QOpenGLShaderProgram

    对shaderProgram进行封装，将source放进文本中。

    ```c++
    /*		未使用封装类前		*/
    //创建shader
    glCreateShader
    //绑定源码
    glshaderSource
    //进行编译
    glCompileShadaer
    //连接
    glCreateProgram glAttachShader glLinkProgram
    ```

    QOpenGLShaderProgram提供了读取源的方式：

    ```c++
    bool QOpenGLShaderProgram::addShaderFromSourceCode(QGLShader::shaderType type,const char* source);
    //从字符串中读取
    //shaderType有Vertex Fragment等
    
    bool QOpenGLShaderProgram::addShaderFromSourceFile(QGLShader::shaderType type,const QString& fileName);
    //从文件中读取
    //文件可以以后缀vert frag 形式用代码存储，放在qrc资源文件中。
    
    virtual bool link();
    bool bind();
    QString log()const;
    ```

* 着色器中的代码相关

    ```c++
    //.frag
    #version 330 core
    //开头：版本号
    out vec4 FragColor;
    in vec4 vertexColor;
    //关键字in out 代表输入输出，用作在流水线中传递变量。
    uniform vec4 ourColor;
    //uniform 进行全局属性的定义，使用后可以在着色器程序的任意阶段被访问。
    //若声明了一个uniform变量，却从未使用过，编译器会自动进行优化移除该变量。
    void main()
    {
        //FragColor= vec4(1.0f, 0.6f, 0.4f, 1.0f);
        FragColor=vec4(ourColor.xyz,1.0f);
    }
    //正文部分进行处理。
    /*
    GLSL中包含了C等其他语言的默认类型：int float double uint bool等。
    有两种容器：Vector 向量 和 Matrix 矩阵
    vector：
    		vecn：默认float，n个数据类型的向量容器。
    		bvecn、ivecn、uvecn、dvecn：相应的属性类型
    		
    向量允许“重组”
    		vec2 v =vec2(0.5,0.7);
    		vec2 v2=vec4(v,0.0,0.0);
    		vec4 v3=vec4(v2.xyz,1.0);
    */
    
    
    //.vert
    #version 330 core
    layout (location = 0) in vec3 aPos;
    //layout:用于连接到顶点属性，location其实就是index的值
    /*
    	可以忽略layout，在代码中使用glGetAttribLocation查询属性位置的值，通过glBindAttribLocation进行绑定。 但不推荐。
    	在cpp文件中进行获取 aPos：
    	--------在着色器中定义标号
    	shaderProgram.bind();
    	GLint posLocation=shaderProgram.attributeLocation("aPos");
    	glVertexAttribPointer(posLocation,3,GL_FLOAT_GL_FALST,3*sizeof(float),(void*)0);
    	--------
        -------- 往着色器中传入标号
        shaderProgram.bind();
        GLint posLocation=2;
        shaderProgram.bindAttributeLocation("aPos",posLocation);
        --------
        
    关联的是VAO中属性的标号，当双方的值相等时，“数据流通”。
    */
    out vec4 vertexColor;
    
    void main()
    {
       gl_Position = vec4(aPos.x,aPos.y,aPos.z,1.0);
       vertexColor=vec4(0.5,0.0,0.1,1.0);
    }
    
    ```

    

## 一个简单的应用-C++（1）

```c++
//引入GLFW库中的头文件
//需要先配置好静态库的连接    ---可以在工程下创建一个单独的库文件夹引入
#include <GLFW/glfw3.h>

int main(void)
{
    // 声明一个GLFWwindow类型的指针，用于存储窗口对象 
    GLFWwindow* window;

	// 初始化GLFW库，如果失败则返回-1并退出程序
    if (!glfwInit())
        return -1;

	// 创建一个640x480像素的窗口，标题为"Hello World"，如果失败则释放GLFW资源并返回-1
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

	// 将窗口的上下文设置为当前线程的上下文
    glfwMakeContextCurrent(window);

	// 在窗口没有关闭的情况下循环执行以下代码
    while (!glfwWindowShouldClose(window))
    {
        // 清除颜色缓冲区，用背景色填充窗口
        glClear(GL_COLOR_BUFFER_BIT);
		 // 开始绘制三角形图元
        glBegin(GL_TRIANGLES);
         // 指定三角形的三个顶点坐标，范围是[-1, 1]
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.0f, 0.5f);
		 // 结束绘制图元
        glEnd();
        
         // 交换前后缓冲区，更新窗口内容
        glfwSwapBuffers(window);

  		// 处理窗口的事件，例如键盘输入、鼠标移动等
        glfwPollEvents();
    }
    
	// 释放GLFW资源并退出程序
    glfwTerminate();
    return 0;
}

```



## 一个简单的应用-QT（1）

```c++
//.h文件
//  继承自QOpenGLWidget和QOpenGLFunctions_3_3_Core
/*
	重写     void initializeGL()override;
     		void resizeGL(int w, int h)override;
     		void paintGL()override;
*/
#ifndef MYOPENGLWIDGET_H
#define MYOPENGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

class MyOpenGLWidget : public QOpenGLWidget,QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit MyOpenGLWidget(QWidget *parent = nullptr);
protected:
     void initializeGL()override;
     void resizeGL(int w, int h)override;
     void paintGL()override;
signals:
private:

};

#endif // MYOPENGLWIDGET_H
```

```c++
//.cpp
/*
1. 在initializeGL中进行初始化;
2. 创建VAO、VBO对象并赋予ID;
3. 进行绑定;
4. 在显存上开辟一块数据存储;
5. 告知显卡如何使用缓冲区内的属性值（解析）,VAO同时对其进行记录;
6. 开启VAO的特定的属性的可用权;
7. 编写着色器（顶点、片段）;
8. 使用shaderProgram进行连接;
9. 对着色器进行回收;
*/
/*
覆盖式绘制
	背景板： 
	glClearColor(0.2f,0.2f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    从显存中取顶点等进行渲染绘制：
	glUseProgram(shaderProgram);
    //绑定VAO，从VAO中取点进行绘制
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES,0,3);
*/





#include "myopenglwidget.h"

MyOpenGLWidget::MyOpenGLWidget(QWidget *parent)
    : QOpenGLWidget{parent}
{


}

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "gl_Position = vec4(aPos.x,aPos.y,aPos.z,1.0);\n"
                                 "}\n\0";

const char * fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "FragColor= vec4(1.0f, 0.2f, 0.4f, 1.0f);\n"
                                   "}\n\0";
float vertices[]={-0.5f,-0.5f,0.0f,0.5f,-0.5f,0.0f,0.0f,0.5f,0.0f};
unsigned int VBO,VAO;
unsigned int shaderProgram;
void MyOpenGLWidget::initializeGL()
{

    initializeOpenGLFunctions();

    //创建VAO、VBO对象并赋予ID；

    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);

    //绑定VAO、VBO对象（通过ID绑定）
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    //为当前绑定到target的缓冲区对象创建一个新的数据存储
    //如果data不是Null，则使用data的数据初始化数据存储。
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

    //告知显卡如何使用缓冲区内的属性值（解析）
    //使用标号为0的属性，3个内容，每个内容3个float，偏移量为0，不需要标准化。
    //VAO同时对其进行记录
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);

    //开启VAO中第一个属性值
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);


    //编写顶点着色器
    unsigned int vertexShader=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
    glCompileShader(vertexShader);

    int success;
    char infolog[512];
    glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader,512,NULL,infolog);
        qDebug()<<"Error:"<<infolog;
    }


    //编写片段着色器
    unsigned int fragmentShader=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader,GL_COMPILE_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader,512,NULL,infolog);
        qDebug()<<"Error:"<<infolog;
    }

    //进行连接
    shaderProgram=glCreateProgram();
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glLinkProgram(shaderProgram);


    glGetProgramiv(shaderProgram,GL_LINK_STATUS,&success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgram,512,NULL,infolog);
        qDebug()<<"Error:"<<infolog;
    }


#if 1
    //已使用完，对着色器进行回收
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
#endif
}

void MyOpenGLWidget::resizeGL(int w, int h)
{

}

void MyOpenGLWidget::paintGL()
{
    glClearColor(0.2f,0.2f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

#if 1
    glUseProgram(shaderProgram);

    //绑定VAO，从VAO中取点进行绘制
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES,0,3);

#endif
}

```

## 一个简单的应用-QT（2）-定时器定时改变颜色

```c++
#include "myopenglwidget.h"

#include <QTime>
#include <QRandomGenerator>
MyOpenGLWidget::MyOpenGLWidget(QWidget *parent)
    : QOpenGLWidget{parent}
{
    m_shaderProgram=new QOpenGLShaderProgram;
    m_timer=new QTimer;
    m_timer->setInterval(200);

    connect(m_timer,&QTimer::timeout,this,[=](){
        timeoutFunc();
    });
}

void MyOpenGLWidget::draw(Shap shap)
{
    m_shap=shap;
    update();
}

void MyOpenGLWidget::timeoutFunc()
{
    makeCurrent();
    int timeValue=QTime::currentTime().second();
    float greenValue=(sin((timeValue+QRandomGenerator::global()->bounded(10,1000)))/2.0f)+0.5f;
    float redValue=(sin((timeValue+QRandomGenerator::global()->bounded(10,1000)))/2.0f)+0.5f;
    float blueValue=(sin((timeValue+QRandomGenerator::global()->bounded(10,1000)))/2.0f)+0.5f;
    m_shaderProgram->setUniformValue("ourColor",redValue,greenValue,blueValue,1.0f);
    doneCurrent();
    update();
}

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "gl_Position = vec4(aPos.x,aPos.y,aPos.z,1.0);\n"
                                 "}\n\0";

const char * fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "FragColor= vec4(1.0f, 0.2f, 0.4f, 1.0f);\n"
                                   "}\n\0";
float vertices[]={-0.5f,-0.5f,0.0f,0.5f,-0.5f,0.0f,0.0f,0.5f,0.0f};
float vertices2[]={0.5f,0.5f,0.0f,
                  0.5f,-0.5f,0.0f,
                  -0.5f,0.5f,0.0f,

                  -0.5f,-0.5f,0.0f,
                  0.5f,-0.5f,0.0f,

                  -0.5f,0.5f,0.0f };
float vertices3[]={-0.5f,0.5f,0.0f,
                   0.5f,0.5f,0.0f,
                   -0.5f,-0.5f,0.0f,
                    0.5f,-0.5f,0.0f};
unsigned int indexs[]={
                0,1,3,
                1,2,3
};



unsigned int VBO,VAO,EBO;
unsigned int shaderProgram;
void MyOpenGLWidget::initializeGL()
{

    initializeOpenGLFunctions();

    //创建VAO、VBO对象并赋予ID；

    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);

    //绑定VAO、VBO对象（通过ID绑定）
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);

    //为当前绑定到target的缓冲区对象创建一个新的数据存储
    //如果data不是Null，则使用data的数据初始化数据存储。
    //glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices2),vertices2,GL_STATIC_DRAW);

    //告知显卡如何使用缓冲区内的属性值（解析）
    //使用属标号为0的性，3个内容，每个内容3个float，偏移量为0，不需要标准化。
    //VAO同时对其进行记录
    //glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);

    //开启VAO中第一个属性值
    glEnableVertexAttribArray(0);


#if 0

    //编写顶点着色器
    unsigned int vertexShader=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
    glCompileShader(vertexShader);

    int success;
    char infolog[512];
    glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader,512,NULL,infolog);
        qDebug()<<"Error:"<<infolog;
    }


    //编写片段着色器
    unsigned int fragmentShader=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader,GL_COMPILE_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader,512,NULL,infolog);
        qDebug()<<"Error:"<<infolog;
    }

    //进行连接
    shaderProgram=glCreateProgram();
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glLinkProgram(shaderProgram);


    glGetProgramiv(shaderProgram,GL_LINK_STATUS,&success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgram,512,NULL,infolog);
        qDebug()<<"Error:"<<infolog;
    }



    //已使用完，对着色器进行回收
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
#endif

    //使用shaderProgram类
    //可以通过返回值来进行验证
    bool success;

    //从字符串中读入源
    //m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,vertexShaderSource);
    //m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,fragmentShaderSource);

    //从文件中读入源

    m_shaderProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex,QString(":/shaders/shapes.vert"));
    m_shaderProgram->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment,QString(":/shaders/shapes.frag"));
   success= m_shaderProgram->link();
    if(!success)
   {
       qDebug()<<"ERR:"<<m_shaderProgram->log();
   }


    //设置绘制模式--正反面，填充方式
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glPolygonMode(GL_FRONT,GL_LINE);
    glPolygonMode(GL_BACK,GL_FILL);

    //默认顺时针是反面，逆时针是正面   GL_CCW 逆时针   GL_CW 顺时针
    glFrontFace(GL_CW); //将顺时针设置为“正面” 同时逆时针变为反面

    //创建EBO，这时候VAO会进行记录所有操作！（包括解绑）
    glGenBuffers(1,&EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indexs),indexs,GL_STATIC_DRAW);


    //设置完毕，解绑
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
    m_timer->start();
}

void MyOpenGLWidget::resizeGL(int w, int h)
{

}

void MyOpenGLWidget::paintGL()
{
    glClearColor(0.2f,0.2f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

#if 0
    glUseProgram(shaderProgram);
#endif
    m_shaderProgram->bind();

    //绑定VAO，从VAO中取点进行绘制  从0开始拿6个数据
    glBindVertexArray(VAO);
   // glDrawArrays(GL_TRIANGLES,0,6);

    //使用EBO进行绘制
    //glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);

    switch (m_shap) {
    case Rect:
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        break;
    default:
        break;
    }

}

```

