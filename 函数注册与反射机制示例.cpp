// 函数注册与反射机制示例.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <map>
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
        static void registerClass(const std::string s, Base* f)
        {
            m_classDictionary[s] = f;
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

    Factory* Factory::m_instance = nullptr;
    std::map<std::string, Base* >Factory::m_classDictionary = {};
    void registerAllClass()
    {
        Factory::getInstance()->registerClass("Novel", new Novel);
        Factory::getInstance()->registerClass("Story", new Story);
    };

}


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





int main()
{
    {
        using namespace func;

        RegisterFunc(testFunction1);
        RegisterFunc(testFunction2);

        CallbackFuncs();
    }//function实现函数注册
    
    {
        using namespace tempClassFunc;
        MyRegistry<Base> registry;
        Base* CLA = new A();
        Base* CLB = new B();

        registry.registerFunc(CLA);
        registry.registerFunc(CLB);

        registry.excuteFunc();
    }//template实现类注册

    {
        using namespace trans;
        registerAllClass();
        Base* novel = Factory::getInstance()->creat("Novel");
        Base* story = Factory::getInstance()->creat("Story");

        novel->print();
        story->print();
    }

    {
        using namespace transPre;
        Instruction* loveUp = InstructionFactory::getInstance()->createInstruction("Instruction_LoveUp");
        Instruction* loveDown = InstructionFactory::getInstance()->createInstruction("Instruction_LoveDown");
        loveUp->soluteInstruction("solution:");
        loveDown->soluteInstruction("solution:");
    }



    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
