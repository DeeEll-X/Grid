## 11.23
+ 添加kill operation: kill args, kill result, parse, core::exec(kill), container:: kill
+ kill  containerID  signal
+ 不知道子进程有没有接收到signal，接收到之后的行为是什么
+ 为container::start()增加修改pid, set state=running, sync()
+ 有一个问题是子进程在后台运行的时候运行状态会变化（退出/退出后新进程复用了进程号）
  + 为restore增加amend行为，如果state=running且pid=0此时修改并sync
  + 对于进程号复用的行为，runc使用另外一个进程来waitpid,我们觉得不太好，用了logfile的文件锁，问题就是exec对应的进程可能会关闭进程号

## 11.24
+ 测试 start需要准备一些必要的参数，为此使用友元类。实验得出friend class cannot be herited，使用类的static方法来实现。
+ 012是顺序开关的，不关fd012是怕库函数的实现出问题： /dev/null用法
+ logFile privilege 644 110 100 100 rwx owner group user； chmod要使用八进制0开头
+ 两个fd指向同一个文件它们交错写时会不会相互覆盖？

## 12.1
+ 增加state 和delete
  + delete先判断容器status，若不在运行，unmount并删除工作目录
  + sudo

## 12.9
+ GRID_CONFIG 记录grid_config.json的路径
+ grid_config.json记录rootdir
+ 各container的rootpath为${rootdir}/${containerid}
+ 各container的bundle由create函数参数传入，记录在status.json文件中
  + status.json的路径为${rootdir}/${containerid}/status.json
+ mntfolder路径为${rootdir}/${containerid}/mntfolder,将bundle内容挂载到该目录下
  + 在create的时候进行

## 12.22
+ 修改：在create时就clone创建多个namespace，并将/proc/[pid]/ns挂载到mntfolder同级目录下，这样在该pid退出时/proc/[pid]/ns不被清理，可以start之后加入这个namespace
+ 在clone出的进程挂载而不是在父进程挂载是因为clone中的CLONE_STOPPED已经deprecated，在父进程挂载时无法保证子进程未退出
+ TODO：测试时create可以运行得到预期挂载结果，但是start后文件系统崩溃


## 12.23
+ 增加hooks
  + 使用map<enum,vector<hookele>>来记录不同hook里的指令
  + 使用enum好处：
    + 原则是尽量减少字面量，好处是容易修改
    + 比较直观
    + 运行时比较耗时较低
    + 从JSON读出string转为enum容易进行错误处理
```
rootdir
  └── containerid
        └── status.json
        └── mntFolder(mount bundle)
        └── writeLayer
bundle
  └── config.json
```
## 12.29
+ 修改namespace：
  + mnt namespace 成功挂载的前提是挂载dst directory是private 的挂载点
  + pid namespace 在setns后，当前进程的pid namespace不发生改变，但是随后fork/clone出的子进程的pid namespace被设置为目标namespace
  + 在clone时设置CLONE_NEWPID,clone出的子进程为一个新的pid namespace的1号进程，1号进程会是该namespace下所有孤儿进程的父进程。一个pid namespace的1号进程一旦退出，操作系统会给该namespace其他所有进程发送SIGKILL。

## 01.03
+ 在create时setup mount。（pause/signal race condition)
+ netns，必须有一个参数。网络设备设置成功，但仍然无法联网。
+ 调用Create Hook时，保持容器运行，并把PID传给hook。（status sync，open， dup2）

## 01.06
+ bridge up 
+ add iptables rule
  + iptables -A FORWARD -o netns0 -j ACCEPT
  + iptables -A FORWARD -i netns0 -j ACCEPT
  + iptables -t nat -A POSTROUTING -s 172.19.0.0/16 ! -o netns0 -j MASQUERADE
  + (sudo iptables -t nat -A PREROUTING -p tcp -m tcp --dport 80 -j DNAT --to- destination 172.18.0.2 : 80)