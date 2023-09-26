# QT信号和槽机制解析

## 前言

* 编译过程

Qt在进行标准编译（如MSVC）前，会进行元对象编译（MOC，元对象编译器），含Q_OBJECT等宏的文件会生成一个moc_[Name].cpp ，其中包含了Q_OBJECT实现的源文件，用于程序运行时的反射。

Qt的编译过程moc-->预处理-->编译-->链接。

* 元对象系统

    Qt的元对象系统主要应用于对象间通信的信号-槽机制，运行时类信息的反射和属性系统。

    每个QObject子类注册Q_OBJECT宏后，都会生成唯一的QMetaObject静态实例，保存了QObject子类的所有类的元信息。



## 机制解析

```c++
QMetaObject::Connection QObject::connect(const QObject* sender, const char* signal ,
                                         const QObject* receiver,const char*method ,
                                         Qt::ConnectionType type=Qt::AutoConnection);
//在代码中通过connection进行静态注册，实现静态链接。
//signal和method通过SIGNAL和SLOT进行包装转换。
```

* Q_OBJECT宏

    ```c++
    /* qmake ignore Q_OBJECT */
    #define Q_OBJECT \
    public: \
        QT_WARNING_PUSH \
        Q_OBJECT_NO_OVERRIDE_WARNING \
        static const QMetaObject staticMetaObject; \
        virtual const QMetaObject *metaObject() const; \
        virtual void *qt_metacast(const char *); \
        virtual int qt_metacall(QMetaObject::Call, int, void **); \
        QT_TR_FUNCTIONS \
    private: \
        Q_OBJECT_NO_ATTRIBUTES_WARNING \
        Q_DECL_HIDDEN_STATIC_METACALL static void qt_static_metacall(QObject *, QMetaObject::Call, int, void **); \
        QT_WARNING_POP \
        struct QPrivateSignal {}; \
        QT_ANNOTATE_CLASS(qt_qobject, "")
    
    /* qmake ignore Q_OBJECT */
    ```

    宏展开后，分为以下阶段：

    ①QT_WARNING_PUSH -----    QT_WARNING_POP 

    在这段代码之间的代码**不触发4793警告**。

    ②static const QMetaObject staticMetaObject等

    **静态元对象，存储该子类的所有数据与相关信息**，返回静态元对象信息。

    ③virtual void *qt_metacast(const char *)

    传入子类对象名进行**上行转换**，将子类转换为基类，在基类的作用域中调用相应函数。

    ④virtual int qt_metacall(QMetaObject::Call, int, void **)

    对信号和槽的实际调用，获取它们的相对id，处理声明的“属性”等，通过公有的获取函数调用私有的静态获取函数，通过二级指针进行表查询，匹配属性、信号、槽等。

    

* 各宏的展开后定义

    ```c++
    signal     ----->    public
        #define  signal  public
    slots      ----->    
        #define  slots
    emit       ----->   
        #define  emit
    SIGNAL     -----> qFLagLocation("2"#a QLOCATION)     //字符串传递信号名
        #define  SIGNAL(x)  qFLagLocation("2"#a QLOCATION)
    SLOT       -----> qFLagLocation("1"#a QLOCATION)
         #define  SLOT(x)  qFLagLocation("1"#a QLOCATION)//字符串传递槽名
    
        
    //空宏的定义的意义
        /*
        通过#ifdef进行判断，执行分支代码，在多平台编译移植时使用，执行不同的逻辑代码。
        也常用于“注册”函数和解释函数的功能。
        */
    ```

* MOC做了什么

    * 找到Q_OBJECT宏，替换展开，生成moc_[name].cpp （此时，子类对象对应 .h .cpp moc_xxx.cpp三个文件）

    * 添加类的静态元对象存储信息，提供获取方法（属性、类信息、成员变量、成员函数的地址+索引表）

    * 实现信号与槽的部分调用逻辑，使用connect可将信号和槽进行绑定调用。

        

* 信号的实现：传入参数a

    * 对a取地址并进行**强制类型转换**，放入一个指针数组（索引表）中。
    * 将指针数组送入**激活方法**中call。（传this、静态元对象、该信号索引、指针数组）

    

    

## 使用场景

单线程下，相当于函数指针的调用。

多线程下，发送者的线程将槽函数的调用转化成一次调用事件放入Qt的事件循环队列，**接收者线程**在下一次事件处理时**调用相应的槽函数**。

参数列表不能有缺省值、模板类、宏定义和函数指针等，实现的是有关字符串的宏展开。

本质上是观察者模式的应用，效率比函数调用慢一个数量级。



## connect

connect的**连接方式**：标准链接、函数指针链接、lambda表达式链接。

connect的**连接类型**：Auto（自动）、Direct（直接）、Queue（队列）、Unique（独立）、BlockingQueue（阻塞队列）。

* **标准链接：**

    通过SIGNAL和SLOT宏函数进行封装，展开后生成字符串，通过connect函数对这俩字符串进行解析，获取信号和槽的绝对索引，然后将信号和槽进行关联。

    所有的**检查在运行时执行**。

    不能调用普通的函数，只能在宏注册的作用域中。

    可设定链接类型，可**重载**信号和槽。----->因为宏包装的内部有形参列表。

* **函数指针链接：**

    通过模板实现，传入信号与槽的地址进行绑定。

    可在代码编写时由编辑器检查（也是编译期的检查）。

    可调用普通成员函数作为槽函数，单不支持两个重载信号（因为没传形参）。（可通过static_cast和QOverload对信号进行转换设置形参从而解决重载问题，但槽函数不能这么做。）

* **Lambda表达式链接：**

    返回的是一个函数指针，包含了信号所对应的函数对象、函数位置、形参列表、函数对象本身。

    不支持信号重载，但信号可通过类型转换解决该问题。static_cast和QOverload。

    默认为直接连接，不可手动设置，即只能在信号发出者的线程中执行。

* **自动链接（Auto）：**

    若信号在接收者所依附的线程内发射，则等同于直接链接；若发送者与接收者处于不同线程中，则等同队列链接。

* **直接链接（Direct）：**

    信号发送时，槽函数直接调用，槽函数在信号发送者的线程内执行。

    不能跨线程传递消息。

* **队列链接（Queue）：**

    当控制权回到接收者所在线程的事件循环时，槽函数被调用，在接收者的线程内执行。

    可跨线程传递消息。

* **独立链接（Unique）：**

    防止重复链接，一个信号多次与一个槽绑定，重复进入事件循环的情况。

* **阻塞队列链接（BlockingQueue）：**

    信号发送之后会一直阻塞，直到关联的槽函数返回。

    信号和槽不能在同一线程中，否则会产生死锁。

    可以用于控制多线程的处理流程。

    

connect函数的**处理流程**：

* 通过SIGNAL和SLOT宏预编译，真正传入的时字符串。
* 处理时，先取出标识符进行对比**检查**（QSLOT_CODE 1    QSIGNAL_CODE 2）；然后获取信号发送者的元对象，提取信号名和参数类型，使用QArgnmentTypeArray容器存储（先创固定的栈内存数组，超过容量后进行动态存储的堆内存数组。）；对信号和槽函数进行检查；进行正式链接（（除开传入connect中的参数外）链接函数中还传入信号与槽函数的静态元对象，去除了const的限制。）--->对回调函数进行提取，正式绑定。

## 元对象

* 元对象的创建方式：Q_OBJECT 宏展开创建，建立代码存在于moc中。

    ```c++
    /* qmake ignore Q_OBJECT */
    #define Q_OBJECT \
    public: \
        QT_WARNING_PUSH \
        Q_OBJECT_NO_OVERRIDE_WARNING \
        static const QMetaObject staticMetaObject; \
        virtual const QMetaObject *metaObject() const; \
        virtual void *qt_metacast(const char *); \
        virtual int qt_metacall(QMetaObject::Call, int, void **); \
        QT_TR_FUNCTIONS \
    private: \
        Q_OBJECT_NO_ATTRIBUTES_WARNING \
        Q_DECL_HIDDEN_STATIC_METACALL static void qt_static_metacall(QObject *, QMetaObject::Call, int, void **); \
        QT_WARNING_POP \
        struct QPrivateSignal {}; \
        QT_ANNOTATE_CLASS(qt_qobject, "")
    
    /* qmake ignore Q_OBJECT */
    ```

* QMetaObject建立类的静态元对象，传入参数：当前信号与槽的调用逻辑（回调函数），当前类对象的元对象信息，元对象索引。

* 内部包含：①类内嵌一个Connection类。

    ​					②当前类名、父类的元对象。

    ​					③提供对信息的偏移量获取、数量获取、索引获取、通过索引获取数据元对象的方法。

    ​					④提供检查传入参数正确性的方法。

    ​					⑤提供信号与槽的链接、断开链接、信号激活的方法。

    ​					⑥提供一个枚举对象Call，用于作为回调函数执行绑定时的策略选择。

    ​					⑦内含有一个结构体，提供当前类的元对象数据的信息和当前类的这些元对象数据的索引表。（名字加长字符串表）

* 作用：存储当前类的信息和提供外部获取的接口。

    

## 设计模式思想

设计模式中提倡模块的高内聚低耦合，侧重于组合而非继承，依赖于接口而非实现。

在Qt的信号与槽机制中，最明显的设计模式有两个：观察者模式和组合模式。

* 观察者模式

    观察者模式用于建立对象与对象之间的通信关系。在该模式中，会改变的是广播器的状态，表示外界信息进行改变；而被动接收的是观察者（订阅者），由广播器进行通知，从而根据通知做出相应的行为。

    * Observer： base - concrete1 -concrete2

        base提供的最核心接口为handleMessage()，用于处理消息。

    * Subuject：base -concrete

        base需要提供对Observer对象的管理的方法，维护一个管理的容器，同时需要一个notify()用于通知消息。

        基础实现：

        ObserverBase：

        ```c++
        #include <iostream>
        
        #pragma once
        class Observer
        {
            public: 
            virtual ~Observer(){};
            virtual void Update(int)=0;
            virtual std::string getName()=0;
        };
        ```

        ObserverConcrete：

        ```c++
        #include "concretesubject.h"
        #include "observer.h"
        #pragma once
        class ConcreteObserver:public Observer
        {
        public:
            ConcreteObserver(Subject* pSubject,std::string name)
            :m_pSubject(pSubject),m_objName(name)
            {
                std::cout<<"observer create "<<getName()<<std::endl;
            };
            ~ConcreteObserver();
            virtual void Update(int);
            virtual std::string getName();
        private:
            Subject *m_pSubject;
            std::string m_objName;
        
        };
        
        
        
        //.cpp
        #include "concreteobserver.h"
        
        void ConcreteObserver::Update(int value)
        {
            //业务逻辑
            std::cout<<"observer:"<<m_objName
                     <<" received info from "<<m_pSubject->getName()
                     <<" state ="<<value
                     <<std::endl;
        
        }
        
        std::string ConcreteObserver::getName()
        {
            return m_objName;
        }
        
        ConcreteObserver::~ConcreteObserver()
        {
        }
        ```

        SubjectBase：

        ```c++
        #include <iostream>
        #include "observer.h"
        #pragma once
        // 信号-被观察者的接口
        class Subject
        {
            public:
            virtual ~Subject(){};
            virtual void registerObserver(Observer*)=0;
            virtual void removeObserver(Observer*)=0;
            virtual void notify()=0;
        
            virtual int getState() const=0;
            virtual void setState(const int & n)=0;
            virtual std::string getName()=0;
        
        };
        ```

        SubjectConcrete：

        ```c++
        #include "subject.h"
        #include <list>
        #pragma once
        class Concretesubject: public Subject
        {
        
        
        public:
            Concretesubject(std::string name):m_objName(name)
            {
                std::cout<<"subject create "<<getName()<<std::endl;
            };
            ~Concretesubject();
            virtual void registerObserver(Observer*);
            virtual void removeObserver(Observer*);
            virtual void notify();
        
            virtual int getState() const;
            virtual void setState(const int & n);
            virtual std::string getName();
        
        private:
            std::list<Observer*> m_observerList;
            int m_nState;
            std::string m_objName;
        };
        
        
        
        //.cpp
        #include "concretesubject.h"
        
        void Concretesubject::registerObserver(Observer * pObserver)
        {
            m_observerList.push_back(pObserver);
            std::cout<<"Concretesubject register:"<<pObserver->getName()<<std::endl;
        }
        
        void Concretesubject::removeObserver(Observer *pObserver)
        {
            m_observerList.remove(pObserver);
             std::cout<<"Concretesubject remove:"<<pObserver->getName()<<std::endl;
        }
        
        void Concretesubject::notify()
        {
            for(auto &it:m_observerList)
            {
                if(it!=nullptr)
                {
                    it->Update(m_nState);
                    std::cout<<"Concretesubject :"<<it->getName()<<"is notified"<<std::endl;
                }
            }
           
        }
        
        int Concretesubject::getState() const
        {
            return m_nState;
        }
        
        void Concretesubject::setState(const int &n)
        {
            m_nState=n;
        }
        
        std::string Concretesubject::getName()
        {
            return std::string();
        }
        
        Concretesubject::~Concretesubject()
        {
        
        }
        ```

        main：

        ```c++
        #include "concreteobserver.h"
        #include "concreteobserver.h"
        #include <iostream>
        
        int main()
        {
            Subject * signal =new Concretesubject("SIGNAL");
            Observer * slot1=new ConcreteObserver(signal,"Slot1");
            Observer * slot2=new ConcreteObserver(signal,"Slot2");
            std::cout<<"------------------------"<<std::endl;
            signal->registerObserver(slot1);
            signal->registerObserver(slot2);
            std::cout<<"------------------------"<<std::endl;
            signal->setState(1);
            signal->notify();
            std::cout<<"------------------------"<<std::endl;
            signal->removeObserver(slot1);
            std::cout<<"------------------------"<<std::endl;
            signal->setState(2);
            signal->notify();
        
            delete signal;
            delete slot1;
            delete slot2;
        
            return 0;
        }
        
        ```

        除此之外，还有一个简单的示例在github上：

        

* 组合模式

    组合模式常用于处理在架构内部拥有特定数据结构的组件：将对象组合成树的结构表示部分-整体的层次关系，使用户对单个对象和对组合对象的调用具有一致性。

    核心是将客户代码与复杂对象的操作进行解耦，仅通过复杂对象提供的高度抽象的接口进行内部的详细操作。依赖于多态的递归调用和结构化的初始化流程。

    在分类上，通常有透明组合模式和安全组合模式。

    透明组合模式的接口提供对数据操作方法，可通过方法组合调用子节点（对象树内部）进行操控，但可能产生安全隐患，也因为继承原因拥有大量冗余。

    安全组合模式的接口不用于数据的访问，仅描述一个高度抽象的操作，具体数据操控由对象树向下遍历调用，安全但对Client的限制较大。

    在Qt中，组合模式的思想以QObject对象树的形式出现。每个QObject可设置parent和查看当前child列表，并对对象书进行统一的管理，当节点被析构时，会优先遍历到叶子节点进行析构。

    符合开闭原则：对扩展开放，对修改关闭。

在Qt中，通过q指针和d指针链接两个层次中同类型的类，通过向前声明和宏调用获取双向引用，对子节点开放权限可选择让自己挂载在父节点上，实现单向对象树。

除此之外，还需要注意：①每个QObject都只能有一个QObject父类对象和多个QObject子类对象列表，以形成对象树。②对象树的管理：父对象自动接管子对象，在父对象析构时会自动释放关联的子对象的内存和节点。③重设父节点表示对象树中节点的移动。④没有父对象的在栈区创建，有父对象的在堆区创建。



## Qt的属性系统

Qt的属性系统基于元对象系统存在，实现的是一种反射机制。

在编码时通过Q_PROPERTY进行设置，在类的实例创建时添加属性，可被Qt的元对象系统操作。

```c++
Q_PROPERTY(type name, [READ read(),WRITE write(),RESET reset(),NOTIFY notify()])
    //type 属性的类型和名称
    //READ等： 对属性的操作和默认调用的函数
    //这是静态属性的注册。
```

* 使用方式

    ①当前类对象公有继承QObject类，使用Q_OBJECT宏。

    ②编码位置位于Q_OBJECT宏之后，构造函数之前。

    ③通常是将类中已有的成员变量设置为“属性” （protected模式）

    ```c++
    Q_PROPERTY(int score READ getScore WRITE setScore NOTIFY scoreChanged)
    protected:
    	int m_score;
    public:
    	int getScore() const;
    	void setScore(const int& score);
    signals:
    	void scoreChanged(const int& score);
    ```

    通过Q_OBJECT设置的属性为静态属性。

* 动态属性

    ①在UI设计师中添加属性进行设置。

    ②在代码中通过setProperty("name",value)进行。对已有的属性的操作是设置、改变；对没有的属性的操作是注册、动态添加。

    ③在定义后通过property("name").to[Value]进行获取。

    



