# C++函数注册与反射机制

## 前言

​		函数注册是一项非常实用的方法调用的技巧，其技巧核心是利用C++中函数表（global）和类的静态方法（Static）提供的指针机制直接访问唯一函数所在地址进行调用。如何去获得该地址，如何在外部某个代码段落中进行调用，这是注册所应该讨论的问题。

​		反射的最大的作用是进行序列化和反序列化一个结构体，以实现各个模块之间的通信和交互。

​		在C++中并没有内置的反射机制，所以当我们需要对项目进行可扩展性的提升时，往往需要自己实现反射机制。

## 实现方案

### 0.用到的头文件

```cpp
#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <map>
```



###  1.Function容器实现

#### Function介绍

​		std::function是C++11开始支持的特性，对标C函数指针的存放，本质是一个模板类重载了()操作符：

```cpp
template<typename T, typename Tp>
class myFunction<T(Tp)> {
   ...
   public:
       T operator()(Tp){
           return ...;
       }
};
// 定义一个含一个参数的function模板
```

​		function是一个包装类，可以用于接收普通函数和函数类对象（即类中显示调用operator()进行()的重载的实现），具体使用方式为：

```cpp
#include <iostream>
#include <functional>

void Func1(){
    std::cout << "This is Func1" << std::endl;
}

void Func2(){
    std::cout << "This is Func1" << std::endl;
}

int main()
{
    std::function<void()> funcP(&Func1);
    funcP();			//"This is Func1"

    funcP = &Func2;		
    funcP();			//"This is Func2" 

    return 0;
}

//容器存放一个函数指针，此处为全局函数Func1和Func2。
//容器调用时采用操作符重载()，此处为FuncP();
```

#### 实现函数的注册与调用

​		通过Function实现的函数注册，通常用于模块间的函数共享与回调机制的建立。其主要步骤如下：

*  使用function对同一返回类型和参数列表的函数进行泛化。

* 维护一个存储该function的容器（vector、List、map等）。

* 一个注册函数Register(function<T>)，传入函数的地址，将函数地址存入容器中。（模拟 “注册”）

* 一个调用函数Callback()，用于调用容器内函数。

* 先对函数进行注册，再执行调用。

    相关示例代码如下： 
    
    1. function部分

```cpp
namespace func {

    typedef std::function<void()>  Func;
    std::vector<Func> g_funcList;

    void RegisterFunc(Func func)
    {
        g_funcList.push_back(func);
    }
    void CallbackFuncs()
    {
        for (auto func : g_funcList)
        {
            func();
        }
    }


    void testFunction1()
    {
        std::cout << "This is testFunction1" << std::endl;
    }
    void testFunction2()
    {
        std::cout << "This is testFunction2" << std::endl;
    }

}
```

​		2.main()部分

```cpp
{
        using namespace func;

        RegisterFunc(testFunction1);
        RegisterFunc(testFunction2);

        CallbackFuncs();
}//function实现函数注册
    
```

​		3.输出部分

```cpp
//输出：
/*
This is testFunction1
This is testFunction2
*/
```



### 2.Template模板类的注册

#### Template介绍

​		template模板是一个拥有高抽象度的C++的方法，提供一种比类更泛用的代码框架。模板是泛型编程的基础，是创建泛型类或函数的蓝图。以下是一个模板示例：

```cpp
#include <iostream>
template<typename T> void swap(T &a, T &b) {
	T tmp{a};
    a = b;
	b = tmp;
}
int main()
{
	int a = 1;
    int b = 2;
	swap(a, b); // 使用函数模板
	std::cout << "a=" << a << ", b=" << b << std::endl;
    
	double c = 1.0; 
    double d = 2.0; 
    swap(c, d);
    std::cout << "c=" << c << ", d=" << d<< std::endl;
    
    return 0;
}

//模板在编译期间自动生成需要的方法。T会自动转换成相应的类。
```

#### 实现模板类注册和调用

​		通过模板类实现函数的注册和调用，主要步骤如下：

* 建立注册类的模板，需要实现注册、调用具体方法和维护一个容器的功能。

* 在模板中调用的方法应该是接口类的方法，这一块涉及到虚函数列表。所以需要规范设计的类中方法的命名。

* 使用时，建立基类指针的模板，建立基类的指针new出具体子类，通过模板进行注册，通过模板进行调用函数。

    1.function部分
    
    ```cpp
    
    namespace tempClassFunc {
    
        template<typename T>
        class MyRegistry
        {
        public: 
            void registerFunc(T* obj) {
                m_registryList.push_back(obj);
            };
            void excuteFunc() {
                for (auto it = m_registryList.begin(); it != m_registryList.end(); ++it)
                {
                    (*it)->DoOperation();
                }
            };
        private:
            std::vector<T*>m_registryList;
        };
    
        class Base {
        public:
            virtual void DoOperation() = 0;
        };
        class A :public Base
        {
        public:
            virtual void DoOperation() {
                std::cout << "This is A" << std::endl;
            };
        };
        class B :public Base
        {
        public:
            virtual void DoOperation() {
                std::cout << "This is B" << std::endl;
            };
        };
    }
    
    ```
    
    2.main()部分
    
    ```c++
     {
            using namespace tempClassFunc;
            MyRegistry<Base> registry;
            Base* CLA = new A();
            Base* CLB = new B();
    
            registry.registerFunc(CLA);
            registry.registerFunc(CLB);
    
            registry.excuteFunc();
        }//template实现类注册
    ```
    
    3.输出部分
    
    ```cpp
    //输出：
    /*
    This is A
    This is B
    */
    ```
    
    

### 3.仿typeid式实现反射

#### typeid介绍

​		typeid是C++中的一个运算符，用于获取一个表达式的类型信息，其操作对象可以是表达式，也可以是数据类型，该运算符返回一个type_info对象的引用。

​		（type_info可能具有私有的拷贝和赋值，不允许用户自主创建对象，而typeid可能被声明为了友元，因为私有构造不能阻止友元对该类的创建。）

​		typeid拥有成员函数：name()用于返回类型名称；raw_name()用于返回名字编码算法产生的新名称，这部分跟编译原理有关；hash_code()用于返回当前类型对应的hash值，全局唯一。

​		在编译时，编译器会给每个类注册一个全局的type_info,通过调用typeid可以获取该type_info的相关内容。

#### 模拟注册表实现反射

​		通过模拟注册表实现类似typeid的反射机制，主要步骤如下：

* 通过一个注册表，将字符串与实际子类类型关联。

* 当需要实例化子类时，使用字符串查找注册表，调用相应的构造函数创建对象。（构造函数存在在类表中）

* 在运行时可通过此方法动态创建不同类的实例，但注册必须在编码时完成。

    代码示例：

    1.function部分

    ```c++
    namespace trans {
        
        class Base
        {
        public:
            virtual void print() = 0;
            //virtual void showStr() = 0;
        private:
            std::string s;
        };
        class Story :public Base
        {
        public:
            void print() {
                std::cout << "This is Story" << std::endl;
            }
        };
        class Novel :public Base
        {
        public:
            void print() {
                std::cout << "This is Novel" << std::endl;
            }
        };
        class Factory
        {
        public:
            static Base* creat(std::string s)
            {
                auto it = m_classDictionary.find(s);
                return (it->second);
            };
            static void registerClass(const std::string s , Base* f  )
            {
                m_classDictionary[s]=f;
            }
            static Factory* getInstance()
            {
                if (m_instance == nullptr)
                {
                    m_instance = new Factory();
                }
                return m_instance;
            }
        private:
            Factory() {};
            static Factory* m_instance;
            static std::map<std::string, Base* > m_classDictionary;
        };
    
        Factory* Factory::  m_instance = nullptr;
        std::map<std::string, Base* >Factory::m_classDictionary = {};
        void registerAllClass()
        {
            Factory::getInstance()->registerClass("Novel",new Novel);
            Factory::getInstance()->registerClass("Story", new Story);
        };
    
    }
    ```

    2.main()部分

    ```c++
     {
            using namespace trans;
            registerAllClass();
            Base* novel = Factory::getInstance()->creat("Novel");
            Base* story = Factory::getInstance()->creat("Story");
    
            novel->print();
            story->print();
        }
    ```

    3.输出部分

    ```c++
    //输出：
    /*
    This is Novel
    This is Story
    */
    ```

### 4.通过预编译实现反射

#### 预编译功能介绍

​		C/C++程序的编译分为**预处理**、**编译**、**汇编**、**链接**四个阶段，在预处理阶段，会对源文件的伪指令和特殊符号进行处理，产生新的源代码交付给编译器，包括执行预处理指令和宏定义、代码替换和删除多余的空白符和注释。

​		1.常用的预编译指令如下：

```c++
#define   //宏定义命名，定义一个标识符来表示一个常量
#include  //文件包含命令，用来引入对应的头文件或其他文件
#undef    //来将前面定义的宏标识符取消定义
#ifdef    //条件编译
#ifndef   //条件编译
#if       //条件编译
#else     //条件编译
#elif     //条件编译
#endif    //条件编译
#error    //用于生成一个编译错误消息

__DATE__  //当前日期，一个以 “MMM DD YYYY” 格式表示的字符串常量
__TIME__  //当前时间，一个以 “HH:MM:SS” 格式表示的字符串常量。
__FILE__  //这会包含当前文件名，一个字符串常量。
__LINE__  //这会包含当前行号，一个十进制常量。
__STDC__  //当编译器以 ANSI 标准编译时，则定义为 1；判断该文件是不是标准 C 程序。
```

​		2.#和##介绍

​		字符串化运算符#可以将宏参数转换成字符串文本const char[]；标记连接运算符##可以把两个参数连接到一起，其含义就是连接后形成的新的标识符的含义。此处有一个示例：

```c++
//示例
#include <stdio.h>
#define link(n) printf_s( "value" #n " = %d", value##n )
int value9 = 10;

int main()
{
   link(9); 
   return 0;
}
//输出：
/*
value9 = 10
*/
```

#### 使用预编译和工厂类实现反射

​		若希望对项目中某个列表的内容进行扩展，同时希望它具有动态的反射功能，比如向已有项目中添加一个新的instruction子类作为新的指令，并在外部文本中更改需要执行的指令名。instruction子类调用项目中其他模块的接口执行一些功能，同时在项目运行时能够直接执行这个指令。

​		这样的功能很像插件系统的扩展，不过它和插件系统的模板代码并不一样。

​		具体实现的示例代码：

​		1.function部分

```c++
namespace transPre {

    class Instruction
    {
    public:
        virtual void soluteInstruction(std::string s) {};
    };
    class Instruction_LoveUp :public Instruction
    {
        void soluteInstruction(std::string s)
        {
            std::cout << s << " by Instruction_LoveUp" << std::endl;
        }
    };
    class Instruction_LoveDown :public Instruction
    {
        void soluteInstruction(std::string s)
        {
            std::cout << s << " by Instruction_LoveDown" << std::endl;
        }
    };

    class InstructionFactory {
    public:
        static InstructionFactory* getInstance()
        {
            if (m_instance == nullptr)
                m_instance = new InstructionFactory();
            return m_instance;
        };
        void registerInstruction(const std::string& insName, Instruction* insClass)
        {
            m_insDictionary[insName] = insClass;
        }
        Instruction* createInstruction(const std::string& insName)
        {
            auto it = m_insDictionary.find(insName);
            if (it != m_insDictionary.end())return it->second;
            else return nullptr;
        }
    private:
        InstructionFactory() {};
        static InstructionFactory* m_instance;
        std::map<std::string, Instruction*> m_insDictionary;
    };
    InstructionFactory* InstructionFactory::m_instance = nullptr;

    class InstructionRegister
    {
    public:
        InstructionRegister() {};
        InstructionRegister(const std::string& insName, Instruction* insClass) {
            InstructionFactory* factory = InstructionFactory::getInstance();
            factory->registerInstruction(insName, insClass);
        };
    };

#define REGISTER_INSTRUCTION(insName)\
static Instruction * createInstruction##insName() \
{ \
    Instruction* obj =new insName(); \
    return obj; \
    } \
static InstructionRegister insRegister##insName(#insName,createInstruction##insName())

//此处的宏注册可通过分文件的方式放置到相应的子类.h文件中

    REGISTER_INSTRUCTION(Instruction_LoveUp);
    REGISTER_INSTRUCTION(Instruction_LoveDown);

}
```

​		2.main()部分

```c++
 {
      using namespace transPre;
      Instruction* loveUp = InstructionFactory::getInstance()->createInstruction("Instruction_LoveUp");
      Instruction* loveDown = InstructionFactory::getInstance()->createInstruction("Instruction_LoveDown");
      loveUp->soluteInstruction("solution:");
      loveDown->soluteInstruction("solution:");
}
```

​		3.输出部分

```c++
//输出：
/*
solution: by Instruction_LoveUp
solution: by Instruction_LoveDown
*/
```

