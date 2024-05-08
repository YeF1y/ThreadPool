# C++11 使用单例模式实现跨平台线程池

## 大致实现过程如图：

![image-20240508180729441](C:\Users\DELL\AppData\Roaming\Typora\typora-user-images\image-20240508180729441.png)

## 主要组件：

使用 C++11 标准库中的 std::thread、std::mutex、std::condition_variable、std::function 和 std::queue 等组件实现。

## 实现过程：

定义了一个 ThreadPool 类，使用单例模式，禁用了构造函数和等号，并且在初始化函数中创建了指定数目的线程。在每个线程中，我们不断地从任务队列中获取任务并执行，直到线程池被停止。在 enqueue() 函数中，我们将任务封装成一个 std::function 对象，并将它添加到任务队列中。在 ThreadPool 的析构函数中，我们等待所有线程执行完成后再停止所有线程。

在主函数中，我们实例化一个 ThreadPool 对象，并向任务队列中添加了 8 个任务。每个任务会输出一些信息，并且在执行完后等待 1 秒钟。由于线程池中有 4 个线程，因此这 8 个任务会被分配到不同的线程中执行。在任务执行完成后，程序会退出。
