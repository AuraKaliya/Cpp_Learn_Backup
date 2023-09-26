# Linux学习

## 简介

Linux操作系统是面向服务器的操作系统。

Linux没有盘符，所有文件都存于根目录下“/”（左斜杠），绝对路径表示中，“/”为根目录+层级关系。

## 命令

Linux中命令是可执行程序，通过命令行进行调用。

* ls

    以列表的形式显示[目录]下的内容。

    ls [-a|-l|-h] [Linux目录]

    ```c++
    -a：all，展示当前/指定目录下所有文件（包括隐藏文件）
        //以“.”开头的文件会自动隐藏
    -l：以竖向列表的形式展示内容，可展示更多信息（权限、用户、组、时间等）
    -h：额外展示文件大小（单位KMG），需要配合-l使用。
    eg：  ls -l /home/aurakaliya 
        //HOME:每个Linux用户在Linux系统中都存在个人账户目录，路径为/home/[用户名]
        //用户默认设置中，工作目录为HOME目录
        
        //文件权限信息：【1】【3】【3】【3】
        //文件描述符【-|d|l】（文件|文件夹|软连接）
        //用户权限【rwx-】
        //用户组权限【rwx-】
        //其他用户权限【rwx-】
    ```

* cd

    用于切换工作目录到指定[目录]路径。

    cd [Linux目录]

    ```c++
    //直接执行时，切换到用户的HOME目录
    //相对路径：以当前目录为起点描述路径，无需以“/”根目录开头/
    //绝对路径：以根目录为起点向后寻找，以“/”开头
    //特殊路径符：   “.”表示当前目录      eg： cd ./Desktop 与 cd Desktop一样
    // 			   “..”表示上一级目录 	 eg： cd ..    cd ../..
    //              “~”表示HOME目录  	eg： cd ~   cd ~/codes
    ```

* pwd

    用于显示当前工作目录。

    pwd

    ```c++
    eg：
    aurakaliya@AuraKaliye:~/codes$ pwd
    /home/aurakaliya/codes
    ```

* mkdir  

    用于创建指定[目录/文件夹]

    mkdir [-p] [Linux路径]

    ```c++
    -p：表示自动创建不存在的父目录（用于创建新的文件夹链）
        //创建文件夹时需要修改权限，确保当前操作在HOME内进行
    eg:
    mkdir -p codes/hahaha/abcde
    ```

* touch

    用于创建指定[目录]下的[文件]

    touch [Linux路径]

    ```c++
    eg： 
        touch main.cpp test.txt
        //用ls -l 展示时，第一个描述符为“d”表示文件夹，“-”表示文件
    ```

* cat

    用于查看指定[目录]下的[文件]内容。

    cat [Linux路径]

    ```c++
    -n：显示行号
        //直接显示全部内容
    ```

* more

    用于查看指定[目录]的[文件]内容，可翻页。

    more [Linux路径] 

    ```c++
    -n： 显示行号
        //分页显示部分内容，可翻页查看
    ```

* less

    用于查看指定[目录]的[文件]内容，可翻页，可向前翻页。

    less [Linux路径]

    ```c++
    -n： 显示行号
        //分页显示部分内容，可翻页查看
        //按q退出
    ```

* cp

    复制[文件/文件夹]到指定[目录]。

    cp [-r] [参数列表]

    ```c++
    -r ：用于复制文件夹，表示递归选取（若文件夹中有文件，则需要使用）
        //前参数表示被复制的路径/文件，最后一个参数表示目标目录
    ```

* mv

    用于[文件/文件夹]的移动或更名。

    mv [参数列表]

    ```c++
    //前参数表示被移动的路径/文件，最后一个参数表示目标目录
    //若目标不存在则进行更名，但不能凭空创建文件夹
    ```

* rm

    用于删除[文件/文件夹]

    rm [-r|-f] [参数列表]

    ```c++
    -r：用于递归索引文件夹的内容进行删除
    -f：表示强制删除，不弹出信息（管理员用户删除内容会有提示，普通用户删除不会有）
        //rm指令支持通配符进行模糊匹配
        //“*”表示多字符匹配
        //“？”表示单字符匹配
        //"[]"表示正则式 [a-z][abABC]用于代替charset中任意值。
    ```

* which

    用于查找可执行[命令]的程序文件存放位置。

    which [命令]

    ```c++
    eg：
        aurakaliya@AuraKaliye:~/codes$ which code
    /home/aurakaliya/.vscode-server/bin/abd2f3db4bdb28f9e95536dfa84d8479f1eb312d/bin/remote-cli/code
            //查找code指令的存放位置，直接使用code表示打开vscode
            //Linux命令的本质是二进制可执行文件，系统命令文件存放于/usr/bin中
    ```

* find

    以指定目录为起始路径，按要求查找所有符合条件的文件。

    find [Linux路径（起始）] [-name] ["文件名"]

    find [Linux路径（起始）] [-size] [+/- n[KMG]]

    find [Linux路径（起始）] [-type] [f|d]

    ```c++
    eg：
        find ~ -name "test"
        find ~ -size  +500M
        //注意查找时的访问权限
        //sudo passwd root 设置root用户 
        //su root 切换root用户
        
        find . -type f -name "*" 
    	//只显示file
    	find . -type d -name "*" 
        //只显示dir
    	find . -maxdepth 1 
        //只显示当前层
    ```

* grep

    通过关键字过滤指定[文件]中的[文件行]

    grep [-n] [关键字] [Linux路径]

    ```c++
    -n：表示在结果中显示匹配行的行号
    -i：忽略大小写匹配
    -v：反向查找
    -l：只打印匹配文件名
    -c：纸打印匹配行数
        
    //可查找多个文件
    //关键字一般用“”将其包括起来
    //Linux路径表示要过滤的文件路径，也可以作为输入内容的输入端口。
    eg：
     grep -n "void" test.txt
        
        //取以void开头的
    aurakaliya@AuraKaliye:~/cppdir3$ grep ^void  main.cpp
    void func_cps(unsigned  n,const Base& context)
        //取包含void 的第一个匹配行
    aurakaliya@AuraKaliye:~/cppdir3$ grep -m 1 "void" main.cpp
        virtual void operator() (unsigned  int)const=0;
    	//取以void 开头的第一个匹配行
    aurakaliya@AuraKaliye:~/cppdir3$ grep -m 1 "^void" main.cpp
    void func_cps(unsigned  n,const Base& context)
        //取包含v、o、i、d的第一个匹配行
    aurakaliya@AuraKaliye:~/cppdir3$ grep -m 1 [^void]  main.cpp
    #include <iostream>
        //判断当前文件是否不存在struct字符串，若不存在则输出文件名。
    aurakaliya@AuraKaliye:~/cppdir3$ grep -L "struct"  test.txt
    test.txt
        
    ```

* wc

    用于统计文件中的函数、单词数量等。

    wc [-c|-m|-l|-w] [Linux路径]

    ```c++
    -c：char统计Bytes数量
    -m：meta统计字符数量
    -l：line统计行数
    -w：word统计单词数量
    //Linux路径表示要统计的文件路径，也可以作为输入内容的输入端口。
        wc main.cpp
    - 76 137 1588 main.cpp
    //行数 单词数 字节数
    ```

* |

    管道符，将左边命令的结果作为右边命令的输入。

    ```c++
    cat main.cpp | grep void -n	  
    ls -lh|grep .cpp			//当前列表下.cpp文件的列表
    ls -lh|grep .cpp |wc -l		//当前列表下.cpp文件数
    cat main.cpp |grep void |grep virtual  //进行嵌套
    ```

* echo

    用于在命令行内输出指定内容，复杂的内容以“”括住。

* ``

    反引号，将字符作为命令执行。

    ```c++
    eg:
    aurakaliya@AuraKaliye:~$ echo “哈哈哈哈哈：s：`pwd`”
    “哈哈哈哈哈：s：/home/aurakaliya”
    ```

* ">" ">>"

    重定向符，“>”表示将左侧命令的结果覆写至右侧指定文件中；“>>”表示将左侧命令的结果追加至右侧的指定文件中。

* tail

    查看文件尾部内容，可以跟踪文件最新修改。

    tail [-f -[num]] [Linux路径]

    ```c++
    -f： 表示持续跟踪，使用此命令后当前命令行将持续跟踪指定文件的修改情况
    -[num] 表示查看尾部行数，默认10行
    
    ```

* ps

    查看当前进程。

    

* exit

    执行退出系统。

* man 

    查看指定命令的帮助。

    man [命令]

    ```c++
    
    ```

* id

    查看当前用户的id，显示当前uid、gid等。

* chmod

    修改文件的权限。

    chmod [rwx | 1+2+4 | ugo=] [file] [-r]

    ```c++
    
    ```

* su

    切换用户

    su [-] [用户名]

    ```c++
    -：在切换用户后是否加载环境变量
    //默认则切换到root用户中，root用户切换成普通用户不需要密码
    ```

    

* sudo

    为普通命令授权，让该命令临时以root权限执行。

    sudo [其他命令]

    ```c++
    //使用sudo前需要为用户配置sudo认证
    ```

    

* groupadd   groupdel

    创建/删去用户组。

    ```c++
    //需要在root权限下执行
    ```

    

* useradd

    创建用户

    useradd [-g|-d] [用户名]

    ```c++
    -g： 指定用户组
        //若不指定，则创建同名用户组并加入
    	//对于已存在的同名用户组，必须用-g进行指定
    -d：指定用户的HOME路径
        //若不指定，则默认为/home/用户名
    ```

* userdel

    删除用户

    userdel [-r] [用户名]

    ```c++
    -r：删除时同时删除用户的HOME路径
        //不使用则HOME会被保留
    ```

* id

    查看指定用户。

    id [用户名]

* usermod

    将指定用户加到指定组上

    usermod [-aG] [用户组] [用户名]

* getent

    查看当前系统中的用户/用户组信息。

    ```c++
    //用户名：密码：用户ID：组ID：描述信息：HOME目录：执行终端
    getent group 查看组信息
    ```

* chmod/chown

    修改权限

    chown [-R] [用户]:[用户组] [文件|文件夹]

    ```c++
    chown root test.txt    将文件的所属用户修改为root
    chown :root test.txt  将文件的所属用户组修改为root
    chown -R root:aurakaliya test   递归处理
    ```

    

* systemctl

    用于控制服务的启动、停止、开机自启

    systemctl [start|status|enable|disable] [服务名]

    ```c++
    start: 启动
    status：状态
    enable：开机自启
    disable：关闭自启
    ```

* ln

    创建软连接（将文件/文件夹连接到其他位置）

    ln -s [参数1 参数2]

    ```c++
    -s 创建软连接
    //作为被连接对象的路径不能为相对路径
    ```

* date

    显示时间

    date [-d] [+格式化字符]

    ```c++
    -d： 按给定的字符串显示时间，可用于时间计算
    //格式化信息：
        //	+%Y-%m-%d  %H-%M-%S
        //	年-月-日  时-分-秒
        //2023-09-23 20-53-53
        
    date -d "+1 year" 显示下一年时间
    ```

    

* ifconfig

    显示/查看当前的ip地址

    ```c++
    //其中，inet表示联网地址，lo表示回环地址
    //0.0.0.0 表示任意地址，对内设置时可表示本机地址，对外通信时表示允许任何计算机访问。
    ```

    ip add也能查看当前ip地址。

* hostname

    查看当前主机名。

* hostnamectl

    主机名控制。

    hostnamectl set-hostname [name]

    ```c++
    hostnamectl set-hostname [name]
    //设置主机名
    ```

* ping 

    连通主机进行通信。

    ping [-c num] [ip|主机名]

    ```c++
    -c：检查网络是否连通，设定检查次数。
    ping -c 4 baidu.com
    ```

* wget

    非交互的文件下载器，可在命令行内直接下载网络文件。

    wget [-b] [url]

    ```c++
    -b： 可选，在后台下载，会将日志写入当前工作目录，通过tail监控日志文件可观察下载进度。
    url：下载链接。
        //不论下载是否成功，文件都会创建。
    ```

* curl

    用于发送http网络请求，用于下载文件、读取网页等。

    curl[-O] [url]

    ```c++
    -O：用于下载文件。当url是下载链接时，通过此选项保存文件。
    ```

* namp

    查询对外暴露的端口。

    nmap [ip]

    ```c++
    nmap 127.0.0.1
    //查自己
    ```

* netstat 

    查指定端口/进程占用情况。

    netstat [anp] |grep []

    

* ps 

    查看进程

    ps [-e -f]

    ```c++
    -e：显示全部的进程。
    -f：以完全格式化的方式显示信息。
        UID   	--	  PID	--  PPID   --  C    --    STIME    --      TTY        --     CMD
     启动进程的用户 -- 进程号 -- 父进程 -- CPU占用率 --进程启动时间 --  ？表示非终端启动 -- 启动路径或命令 
    ```

* kill

    通过kill消息关闭进程。

    kill [-9] [进程ID]

    ```c++
    -9：强制关闭进程。
        //   terminated ；
        //   killed ；
    ```

    

* 



## 其他杂记

只要向一个不存在的文件进行输入，都会创建该文件。

终端文件：/dev/tty。

黑洞文件：/dev/null。

进程处于睡眠状态时，不影响异步信号。

root用户拥有系统的最大权限，普通用户一般在HOME目录内不受限，其他地方一般只有只读、执行权限。

切换用户后，通过exit命令或Ctrl+d快捷键回退到上一个用户。

在Linux中，可配置多个用户，也可配置多个用户组，一个用户可以加入多个用户组。

Linux的权限分为用户权限和用户组权限。

当访问网址时，会先查看/etc/host文件内是否具有记录网址的ip地址，再联网询问公网DNS服务器是否有记录，根据结果打开网站或确认网址不存在。（域名解析服务）

Linux支持65535个端口，其中1-1023通常为系统内置或默认程序的预留端口。



