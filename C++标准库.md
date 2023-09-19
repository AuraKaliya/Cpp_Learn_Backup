# C++标准库

* #### std::transform

    std::transform是C++标准库中的一个通用函数，它的概念是将一个给定的函数应用到一个序列的每个元素上，并将结果存储到另一个序列中。通常用来实现一些转换、映射、投影等操作。

    * 使用方法

        有**两种**形式，一种是接受一个**一元操作符**，另一种是接受一个**二元操作符**。一元操作符是指只需要一个参数的函数，二元操作符是指需要两个参数的函数。例如，std::toupper是一个一元操作符，它将一个字符转换为大写；std::plus是一个二元操作符，它将两个数相加。

        需要三到四个**迭代器**、一个**函数或函数对象**作为参数，表示输入序列的起始和终止位置，输出的起始位置和应用到序列元素上的操作。

        **不会改变输入序列中的元素**，也不会改变输出序列中已有的元素，只会**覆盖**输出序列中未初始化的元素。因此，**输出序列必须有足够的空间来存储结果**。

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