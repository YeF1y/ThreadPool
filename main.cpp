
/* 使用单例模式结合C++11新特性实现线程池 */

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>

class ThreadPool {
public:
    //禁用构造函数和=
    ThreadPool(const ThreadPool& threadPool) = delete;
    ThreadPool& operator=(const ThreadPool& threadPool) = delete;

    static ThreadPool& GetInstance(){
        static ThreadPool threadPool;//饿汉模式
        return threadPool;
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread& thread : threads) {
            thread.join();
        }
    }

    template<typename F, typename... Args>
    void enqueue(F&& f, Args&&... args) {
        std::function<void()> task(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        {//指定锁的范围
            std::unique_lock<std::mutex> lock(mutex);
            tasks.emplace(std::move(task));
        }
        condition.notify_one();
    }

    void setCount(int numThreads){
        this->count=numThreads;
    }

    int getCount(){
        return this->count;
    }

    void initPool() {
        for (int i = 0; i < getCount(); ++i) {
            threads.emplace_back([this]() {
                while (true) {
                    std::unique_lock<std::mutex> lock(mutex);
                    condition.wait(lock, [this] { return stop || !tasks.empty(); });
                    if (stop && tasks.empty()) {
                        return;
                    }
                    std::function<void()> task(std::move(tasks.front()));
                    tasks.pop();
                    lock.unlock();
                    task();
                }
            });
        }
    }

private:
    std::once_flag once;
    ThreadPool(){};
    int count = 0;
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex mutex;
    std::condition_variable condition;
    bool stop;
};

int main() {
    std::mutex tmutex;//主线程锁，防止打印结果时出现句子乱序，仅用于测试，因为会降低性能
    ThreadPool::GetInstance().setCount(4);
    ThreadPool::GetInstance().initPool();
    for (int i = 0; i < 8; ++i) {
        ThreadPool::GetInstance().enqueue([i, &tmutex] {
            {//指定锁的范围
                std::unique_lock<std::mutex> lock(tmutex);
                std::cout << "Task " << i << " is running in thread " << std::this_thread::get_id() << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
            {//指定锁的范围
                std::unique_lock<std::mutex> lock(tmutex);
                std::cout << "Task " << i << " is done" << std::endl;
            }
        });
    }
    return 0;
}