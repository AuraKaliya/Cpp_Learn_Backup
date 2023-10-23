



#C++模板元编程学习



## 特殊技巧

### 让类型模板参数成为友元

实际上是对一个**通用类模板**的**二次封装**。

①**模板类**中提供**通用化的解决方法**和**公开的属性**，并**声明一个类型模板参数为友元**。

②编写具体实例类型，若要使用模板提供的方法，则将本类作为模板参数构建①中定义的模板类作为成员变量（属性）。

③在该具体类中添加方法，方法内调用模板提供的属性和处理逻辑。**本质上是一个封装。**

④在主函数中建立具体类的对象，调用该方法执行。

```c++
template<typename FriendType>
class A
{
    friend class FriendType;
private:
	int data;
}

class B
{
public:
	void func()
    {
        std::cout<<obj.data<<std::endl;
	};
private:
    A<B>obj;
}

void main()
{
    B b;
    b.func();
}
```



### 模板实现的多态

①功能类中包含有某名字的函数/属性。

②模板**函数**的**形参为模板参数**，中调用模板参数的对象的某个名字的方法。

③在某个事件发生时，通过**该模板函数**和传入某个**具体对象**进行**执行对象对应的类内含的方法**。

④本质上是**事件发生**-**事件函数调用+事件对象作为参数传入**的表述。传统的思维是事件发生-找到发生事件的对象进行调用该事件对应的成员函数。

```c++
 class Human
    {
    public:
         void eat()
        {
            std::cout << "人类吃米饭和面食" << std::endl;
        }
        virtual ~Human()
        {
            // 作为父类的析构函数一般都应该是虚析构
        }
    };
    
    class Man
    {
    public:
        void eat()
        {
            std::cout << "男人喜欢吃米食" << std::endl;
        }
    };
    
    class Women
    {
    public:
        void eat()
        {
            std::cout << "女人喜欢吃面食" << std::endl;
        }
    };
    
    template<typename T>
    void eatTnpl(T& obj)
    {
        obj.eat();
    }
    
    void func()
    {
        Man objMan;
        Women objWomen;
        eatTnpl(objMan);
        // 男人喜欢吃米食
        eatTnpl(objWomen);
        // 女人喜欢吃面食
    }
```

模板实现的多态，是一种静态的多态，在**编译时期确定调用的实例**，减少了查虚函数表的性能开销，是**泛型编程的思想基础之一**。

通过事件来关联事件发生对象的行为。

传统多态是运行时多态，包括**虚函数重写**和**同名函数重载**，在运行时期**根据传入的参数，查表确定调用的实例**。



### 模板特化

模板特化是让程序/算法在编译时期进行的优化，以提高运行时的性能。

模板本身就是一种可生成函数/类的一种机制，是一种代码生成技术。

* 一般形式

    ①写出模板的一般形式（原型），作为**静态方法**。

    ②写出模板特化的方法，进行**显式声明**。

    ③（多元偏特化）针对同名模板的不同类型组合进行提前定义规则：如传T和U时，偏特化T与U为同类型时、T与U为指针时......

```c++
//用decltype配合模板，查询是否有“某个名字”的变量。

struct A
{
    int x;
};
struct B
{
    int y;
};

template <typename T>
void foo(const T& t,decltype(T::x)*ptr=nullptr)
{
    std::cout<<"have member x."<<std::endl;
}
template <typename T>
void foo(const T& t,decltype(T::y)*ptr=nullptr)
{
    std::cout<<"have member y."<<std::endl;
}

void main()
{
    struct A a;
    struct B b;
    foo(a);
    foo(b);
}
```

```c++
//用decltype和declval<T>()配合模板，查询是否有“某个名字”的函数。
/*
declval<T>()能返回类型 T 的右值引用参考.
我们常常并不真的直接需要 declval 求值求得的伪实例，更多的是需要借助于这个伪实例来求取到相应的类型描述，也就是 T。所以一般情况下 declval 之外往往包围着 decltype 计算，设法拿到 T 才是我们的真实目的
*/
struct A
{
    int size(){return x;};
    int x;
};
struct B
{
    int size(){return y;};
    int y;
};

template<typename T>
auto foo(T& t)->decltype(declval<T>().size())
{
    return t.size();
}
```



### 萃取

针对模板中的模板参数进行的操作，分为固定萃取、值萃取和类型萃取三类。

* 固定萃取

    给定一个类型，萃取另一个类型，两个类型之间的转换关系确定。

    ```c++
    //将T进行优化，实现类型的自动识别转换。
    
    //通用模板
    template <class T>
    struct sumType{};
    
    //特化萃取
    template<>
    struct sumType<char>
    {
        using type =int;
    }
    //当传入的为char类型时，type对应int。
    template<>
    struct sumType<int>
    {
        using type=int64_t;
    }
    //当传入的为int类型时，type对应位int64_t。
    
    template<class T1>
        auto func(const T1* start,const T1* end)
    {
        using sumT=typename sumType<T1>::type;
        sumT sum{};
        for(;;){
            sum+=(*start);
            if(start==end) break;
            ++start;
        }
        return sum;
    }
    ```

    ```c++
    //实现“退化”:丢弃const引用，数组转化成指针，函数名转化位函数指针。
    /*
    	传入参数为引用等（T1& 或T1&&）
    	--用using type=T1 去掉引用
    	传入参数为const等 （const T1）
    	--用using type=T1 去掉const
    	传入参数为T1、size_t
    	--struct myDecay<T1[size]>{using type=T1*;} 进行退化处理
    	--用时：myDecay<Base[2]>::type value;
    */
    ```

    

* 值萃取

    给定一个类型，得到一个值。

* 类型萃取

    实现对类型信息的提取和处理。

    ```c++
    template<class T1,class T2>
        struct myIsSame<T1,T2>
        {
            using type=bool;
            static type value=false;
    	}
    template<class T1>
        struct myIsSmae<T1,T1>
        {
            using type =bool;
            static type value=true;
        }
    
    //调用
    myIsSame<decltype(a),decltype(b)>::value;
    
    ```

    
