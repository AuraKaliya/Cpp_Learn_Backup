// TemplateTest_10_20_1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class Animal {
public:
    struct Interface {
        virtual std::string toString() const = 0;
        virtual ~Interface() = default;
    };
    std::shared_ptr<const Interface> _p;

public:
    Animal(Interface* p) : _p(p) { }
    std::string toString() const { return _p->toString(); }
};

class Bird : public Animal::Interface {
private:
    std::string _name;
    bool        _canFly;

public:
    Bird(std::string name, bool canFly = true) : _name(name), _canFly(canFly) {}
    std::string toString() const override { return "I am a bird"; }
};

class Insect : public Animal::Interface {
private:
    std::string _name;
    int         _numberOfLegs;

public:
    Insect(std::string name, int numberOfLegs)
        : _name(name), _numberOfLegs(numberOfLegs) {}
    std::string toString() const override { return "I am an insect."; }
};

int main() {
    std::vector<Animal> creatures;

    creatures.emplace_back(new Bird("duck", true));
    creatures.emplace_back(new Bird("penguin", false));
    creatures.emplace_back(new Insect("spider", 8));
    creatures.emplace_back(new Insect("centipede", 44));

    // now iterate through the creatures and call their toString()

    for (int i = 0; i < creatures.size(); i++) {
        std::cout << creatures[i].toString() << '\n';
    }
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
