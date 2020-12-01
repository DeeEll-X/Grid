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