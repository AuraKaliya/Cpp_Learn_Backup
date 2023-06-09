# Qt读取数据库进行数据初始化的方案（动态、静态）

## 动态读取

### 流程简述及代码构建

​		流程：连接数据库-获取目标表名-根据表名找到初始化函数-获取目标表的数据-调用初始化函数进行初始化。

​		动态读取建立在反射机制的基础上，需要利用C++中静态函数在编译期分配地址和空间的特性。基于两种反射机制的建立，有以下两个方案实现动态读取功能。

#### 1. 基于函数指针和map实现的反射功能

​		额外添加：目标类的映射静态类，内含所有需要从数据库读取的数据。

* 目标类：Character

    ```c++
    #ifndef CHARACTER_H
    #define CHARACTER_H
    
    #include <QString>
    #include "characterreference.h"
    #include <QDebug>
    
    #pragma execution_character_set("utf-8")
    
    // GPT的解读
    /*
    这是一个名为Character的类，主要用于存储和管理游戏中角色的信息。该类包含了三个成员变量，分别是int类型的ID、QString类型的name和titleName。其中ID为角色的唯一标识符，name为角色的名称，titleName为角色的头衔。
    
    这个类提供了默认构造函数和带参构造函数。默认构造函数没有参数，不做任何操作。带参构造函数接收一个ReferenceType参数，并重载了该参数类型的构造函数。该构造函数根据传入的参数类型，在构造角色对象时提供相应的数据源，以便初始化角色的ID、name和titleName成员变量。
    
    除了构造函数和成员变量的声明和定义之外，该类还提供了get和set方法用于访问和修改成员变量的值，并且提供了一个showInfo方法，用于打印角色的信息，方便调试。
    
    需要注意的是，该类中的ReferenceType枚举类表示数据源的类型，由于该类中只有一个数据源，因此我们只定义了一个CharacterReference成员对象，如果有多个数据源，可以根据需要增加不同的数据源成员。
    */
    //
    
    enum class ReferenceType{
      CharacterReference,
    };
    class Character
    {
    public:
        Character();
        Character(ReferenceType);
        
        //------------------可去掉-------------------------//
        int ID() const;
        void setID(int newID);
    
        QString name() const;
        void setName(const QString &newName);
    
        QString titleName() const;
        void setTitleName(const QString &newTitleName);
        void showInfo();
    private:
        int m_ID;
        QString m_name;
        QString m_titleName;
    };
    
    #endif // CHARACTER_H
    
    ```

    

* 映射静态类：CharacterReference

    ```c++
    #ifndef CHARACTERREFERENCE_H
    #define CHARACTERREFERENCE_H
    
    #include <QString>
    #include <QSqlQuery>
    #include <QDebug>
    //GPT的解读
    /*
    这是一个名为CharacterReference的类，通过单例模式提供了角色信息的数据源。该类中有三个静态成员变量，分别是int类型的m_ID、QString类型的m_name和m_titleName。这些变量用于存储角色的ID、name和titleName信息。
    
    该类提供了一个getInstance静态方法，返回一个CharacterReference类型的指针，用于获取类的唯一实例。该方法使用了懒汉式单例模式的实现方式，即在第一次调用时才创建实例。在类的内部定义了一个私有构造函数，以保证该类不能被外部直接实例化。
    
    此外，该类还提供了get和set方法用于访问和修改成员变量的值，其中set方法重载了QVariant类型参数的版本，方便从数据库中读取数据并设置相应的成员变量。最后，该类还提供了一个show方法用于在控制台输出角色信息，方便测试和调试。
    
    */
    //
     class CharacterReference
    {
    public:
    
        static CharacterReference* getInstance();
    
        int ID();
        static void setID(int newID);
        static void setIDByQVariant(const QVariant &value);
        QString name();
        static void setName(const QString &newName);
        static void setNameByQVariant(const QVariant &value);
        QString titleName();
        static void setTitleName(const QString &newTitleName);
        static void setTitleNameByQVariant(const QVariant &value);
        static void show();
    private:
        CharacterReference();
    
        static CharacterReference* m_instance;
    
        static int m_ID;
        static QString m_name;
        static QString m_titleName;
    };
    
    #endif // CHARACTERREFERENCE_H
    
    ```



* 读取类：InitByDatabase

    ```c++
    #ifndef INITBYDATABASE_H
    #define INITBYDATABASE_H
    
    #include <QSqlDatabase>
    #include <QSqlQuery>
    #include <QSqlError>
    #include <QDebug>
    #include <QApplication>
    #include <QSqlRecord>
    #include <QString>
    #include <QDebug>
    #include <QMap>
    #include <QVector>
    #include "character.h"
    
    typedef void (*Func)(const QVariant &);
    class InitByDatabase
    {
    public:
        static InitByDatabase *getInstance();
    
        bool initDatabase();
    
        void initDictionary();
        QVector<Character*> readDataToCharacter();
    
    private:
        static InitByDatabase* m_instance;
        InitByDatabase();
        ~InitByDatabase();
        QSqlDatabase db;
    
        QMap<QString,Func> m_initFunctionDictionary;
    
    };
    
    #endif // INITBYDATABASE_H
    
    ```

    ```c++
    #include "initbydatabase.h"
    
    //GPT的解读————————————
    /*
    这段代码实现了一个单例模式的类InitByDatabase，主要用于初始化数据库连接和读取游戏中角色数据。
    头文件#include "initbydatabase.h"表示引用了InitByDatabase类所在的头文件。该类封装了与数据库连接相关的函数。
    InitByDatabase::InitByDatabase()是InitByDatabase类的构造函数，定义为空函数体。
    InitByDatabase::~InitByDatabase()是InitByDatabase类的析构函数，在析构该类对象时关掉数据库连接。
    在InitByDatabase类中使用单例模式，定义静态指针m_instance指向InitByDatabase对象，初始时设为nullptr。
    InitByDatabase *InitByDatabase::getInstance()是获取单例对象的方法，如果还未创建对象，则创建一个初始化连接，并将其赋值给m_instance。
    bool InitByDatabase::initDatabase()方法是初始化数据库连接的函数，使用QSqlDatabase连接MySQL数据库，设置主机名、数据库名、端口号、用户名和密码，并打开连接，返回连接状态。
    void InitByDatabase::initDictionary()方法初始化了一个std::map结构的字典，键为数据表中的列名，值为一个指向字符Reference类中对应成员的函数指针。
    QVector<Character*> InitByDatabase::readDataToCharacter()方法读取数据库中的角色数据并存入QVector容器中，通过QSqlQuery查询每个角色数据的相应列值，再通过字典的函数指针设置角色的成员变量。最后将角色指针存入QVector容器中，返回该容器。
    总之，这段代码的主要功能是实现数据库的连接和数据读取的操作，并封装在InitByDatabase类里以方便调用。其中使用了单例模式和字典（map）数据结构。
    */
    //
    InitByDatabase::InitByDatabase()
    {
    }
    
    InitByDatabase::~InitByDatabase()
    {
        db.close();
    }
    InitByDatabase* InitByDatabase::m_instance = nullptr;
    InitByDatabase *InitByDatabase::getInstance()
    {
        if(m_instance==nullptr)
        {
            m_instance=new InitByDatabase();
        }
        return m_instance;
    }
    
    bool InitByDatabase::initDatabase()
    {
    
        db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName("localhost");
        db.setDatabaseName("luck_no_complete_save1");
        db.setPort(3306);
    
        db.setUserName("root");
        db.setPassword("password");
    
        bool flag=db.open();
        if(!flag)
        {
            qDebug()<<"Cant Link";
        }else
        {
            qDebug()<<"Link";
        }
        return flag;
    }
    
    void InitByDatabase::initDictionary()
    {
        m_initFunctionDictionary["CharacterID"]=CharacterReference::setIDByQVariant;
        m_initFunctionDictionary["CharacterName"]=CharacterReference::setNameByQVariant;
        m_initFunctionDictionary["CharacterTitle"]=CharacterReference::setTitleNameByQVariant;
    
    }
    
     QVector<Character*> InitByDatabase::readDataToCharacter()
    {
        QVector <Character*> tmpCharacterList;
        QString strTableNmae="CHARACTERLIST";
        QVector<QString> columName;
        QString str = QString("select  column_name from information_schema.columns where table_name='%1'and TABLE_SCHEMA='%2';").arg(strTableNmae).arg(db.databaseName());
    
        QSqlQuery query(str,db);
        while(query.next())
        {
            columName.append(query.value(0).toString());
        }
    
        QSqlQuery query1("select * from "+db.databaseName()+"."+strTableNmae+";",db);
        while (query1.next()) {
            for(int i=0;i<columName.size();++i)
            {
                auto it =m_initFunctionDictionary.find(columName[i]);
                if (it!=m_initFunctionDictionary.end())
                {
                    m_initFunctionDictionary.value(columName[i])(query1.value(i));
                }
            }
            Character * tmpCharacter=new Character(ReferenceType::CharacterReference);
            tmpCharacterList<<tmpCharacter;
        }
        return tmpCharacterList;
    }
    
    ```

* 主程序：

    ```c++
     InitByDatabase* ibd=InitByDatabase::getInstance();
    
        ibd->initDatabase();
        ibd->initDictionary();
        QVector<Character*> chara=ibd->readDataToCharacter();
    
        for(auto it:chara)
            it->showInfo();
    ```

* 示例地址：

    ```c++
    https://github.com/AuraKaliya/QT_practice
    //QtLinkMySQLTest
    ```

    

## 静态读取

### 流程简述

​		写死的表、列、初始化程序。连接数据库-获取表内容-根据表内容进行初始化。

​		适合数据结构简单、没有/少更新需求的程序。

​		适合某些特殊情况下表字段变动不大的数据库。

### 相关文章

​		因为太基础了，所以只贴几个博客链接：
​		

```c++
https://blog.csdn.net/zhtsuc/article/details/53785070   //Qt连接mysql查询数据
```

