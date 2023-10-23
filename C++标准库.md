# C++标准库

## STL

​	STL标准库六大件：容器（Container）、迭代器（Iterator）、仿函数（Functor）、算法（Algorithms）、分配器（Allocator）。

### 容器

* Vector

    数组型容器，连续空间，成倍开辟，深度拷贝。

    一个Vector中有三个类型指针：指向对开始位置，指向对结束的位置，指向现有容量的数据，标示了两个部分：实际容量和显示容量。

    * []运算符重载，不会进行越界检查，更高效；.at()成员函数，会进行越界检查，更安全。

    * 通过resize()调整大小，但size变小时，空出的内存不会释放（显示大小变小），size变大时，会开辟一块新内存空间，再将内容拷贝填充进去，这时候过去的迭代器都会失效，需要更新迭代器。

    * clear()等价于resize(0)，只是将size标记为0。通常使用clear()清除后再resize()调整大小更安全。

    * ```c++
        push_back()     //向容器末尾添加一个元素
        pop_back()		//从容器末尾删除一个元素
        back()			//获取容器末尾前一个元素
        front()			//获取容器首个元素
        data()			//获取容器首元素的地址（指针）
        ////////////////////////////
        capacity()		//查询已分配内存的大小（实际大小）
        size()			//查询显示大小
        reserve()		//只能扩容，减容无效。  主动扩容，可预备一定的存储量，但不会初始化。
        shrink_to_fit()	//释放多余的容量，重新分配更小的内存
        ///////////////////////////
        insert(n,x)		//向n位置前插入一个元素，尾部遍历寻找。
        insert(it,n)	//在某迭代器前插入一个元素
        insert(位置，n，x) //一次性找位置，尾部遍历，一次移动n个位置，一次性插入n个重复元素x
        insert(位置，it,it)//可通过迭代器插入容器。
        erase(it,it)	//删除一个区间
        ```

        

* Set
    集合的概念，自动排序（排字符是按ASCII值，先大写后小写顺序），去重、按值高效查找。
    禁止排指针（地址）

    ```c++
    set<T,compT>  	//compT比较函子，默认按<进行排序。
        
    struct	MyComp{
        bool operator()(syring const&a,string const&b)const
        {
            return a<b;
    	}
    };
    ```

    set内部判断元素是否相等：!(a<b)&&!(b<a)。（调用函子进行判断）
    set具有随机访问特性，不能使用iterator+[偏移量]进行访问，只能通过++ --进行迭代。

    ```c++
    insert()	//自动插入、自动去重，返回一个pair<it,bool>代表插入位置和是否插入成功
    /////
    auto [ok,it]=b.insert(3);
    //等价于
    auto tmp=b.insert(3);
    auto ok=tmp.first();
    auto it=tmp.second();
    ////////////////
    find(x)		//查找集合中该元素的位置
    //使用std::prev(it) 得到该迭代器的前一个迭代器
    //使用std::next(it) 得到该迭代器的后一个迭代器
    count(x) 	//只能查找到是否存在某个元素（1或0）
    contains(x)
    erase(x)	//删除某元素，返回删除了的元素的个数
    lower_bound(x)	//找到第一个大于等于x的元素
    upper_bound(x)	//找到第一个大于x的元素
    find(x)			//找到第一个等于x的元素
    ```

    具有排序、不去重的容器：multiset

    

* Deque

    可在双端插入的连续空间，使用表结构进行空间段的记录和访问。

* Stack

* Queue

* List

* Map

* Multimap

* String





```c++
//向命名空间内添加一个函数，能够打印输出vector

namespace std{
    template<class T>
        ostream& operator<<(ostream& os,const vector<T> &v)
    {
        	os<<'{';
        	auto it =v.begin();
        	if(it!=v.end())
            {
                os<<*it;
                for(++it;it!=v.end();++it) os<<','<<*it;
            }
        	os<<'}';
        return os;
	}
}
```





* #### std::transform

    std::transform是C++标准库中的一个通用函数，它的概念是将一个给定的函数应用到一个序列的每个元素上，并将结果存储到另一个序列中。通常用来实现一些转换、映射、投影等操作。

    * 使用方法

        有**两种**形式，一种是接受一个**一元操作符**，另一种是接受一个**二元操作符**。一元操作符是指只需要一个参数的函数，二元操作符是指需要两个参数的函数。例如，std::toupper是一个一元操作符，它将一个字符转换为大写；std::plus是一个二元操作符，它将两个数相加。

        需要三到四个**迭代器**、一个**函数或函数对象**作为参数，表示输入序列的起始和终止位置，输出的起始位置和应用到序列元素上的操作。

        **不会改变输入序列中的元素**，也不会改变输出序列中已有的元素，只会**覆盖**输出序列中未初始化的元素。因此，**输出序列必须有足够的空间来存储结果**。

        * （方法一）传入三个参数：s.begin()、s.end()、t.begin()和处理函子。

            操作结果是从s.begin()开始处理每个元素，传入到处理函子中进行处理，再由处理函子返回结果，将结果存放到t.begin()开始的每个元素中覆盖。

    * 示例

        ```c++
        //将一个字符串转换为大写
        #include <iostream>
        #include <string>
        #include <algorithm>
        #include <cctype>
        
        int main()
        {
            std::string s = "Hello, world!";
            // 创建一个新的字符串，用来存储结果
            std::string t(s.size(), '\0');
            // 使用std::toupper作为一元操作符，将s中的每个字符转换为大写，并存储到t中
            std::transform(s.begin(), s.end(), t.begin(), std::toupper);
            // 输出结果
            std::cout << t << std::endl; // 输出HELLO, WORLD!
            return 0;
        }
        ```

        ```c++
        //将两个向量中的元素相加，并存储到另一个向量中：
        #include <iostream>
        #include <vector>
        #include <algorithm>
        #include <functional>
        
        int main()
        {
            std::vector<int> v1 = {1, 2, 3};
            std::vector<int> v2 = {4, 5, 6};
            // 创建一个新的向量，用来存储结果
            std::vector<int> v3(v1.size());
            // 使用std::plus作为二元操作符，将v1和v2中的每对元素相加，并存储到v3中
            std::transform(v1.begin(), v1.end(), v2.begin(), v3.begin(), std::plus<int>());
            // 输出结果
            for (auto x : v3) {
                std::cout << x << " ";
            }
            std::cout << std::endl; // 输出5 7 9
            return 0;
        }
        ```

        ```c++
        //将一个对象数组中的某个属性提取到另一个数组中
        #include <iostream>
        #include <array>
        #include <algorithm>
        
        // 定义一个结构体类型，表示学生信息
        struct Student {
            int id;
            std::string name;
        };
        
        int main()
        {
            // 创建一个对象数组，初始化一些值
            std::array<Student, 3> students = {{
                {1, "Alice"},
                {2, "Bob"},
                {3, "Charlie"}
            }};
            // 创建一个字符串数组，用来存储结果
            std::array<std::string, 3> names;
            // 使用lambda表达式作为一元操作符，将students中的每个对象的name属性提取出来，并存储到names中
                std::transform(students.begin(), students.end(),names.begin(), [](auto s) {
                return s.name;
            });
            // 输出结果
            for (auto x : names) {
                std::cout << x << " ";
            }
            // 使用lambda表达式作为二元操作符，将students中的每个对象的name属性提取出来，并存储到names中
            std::transform(students.begin(), students.end(),students.begin(),names.begin(), [](auto s,auto s2) {
                return s.name+s2.name;
            });
            // 输出结果
            for (auto x : names) {
                std::cout << x << " ";
            }
            std::cout << std::endl; // 输出Alice Bob Charlie
            return 0;
        }
        ```

        

* 