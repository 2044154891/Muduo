#include "EventLoopThread.h"
#include "EventLoop.h"


EventLoopThread::EventLoopThread(const ThreadInitCallback &cb ,const std::string &name)
    : loop_(nullptr)
    , exiting_(false)
    , thread_(std::bind(&EventLoopThread::threadFunc, this), name) //没有启动
    , mutex_()
    , cond_()
    , callback_(cb)
{

}
                                 
EventLoopThread::~EventLoopThread()
{
    exiting_ = true;
    if(loop_ != nullptr)
    {
        loop_->quit();
        thread_.join();
    }
}

EventLoop *EventLoopThread::startLoop()
{
    thread_.start() ;//启用底层线程Thread类对象thread_中通过start()创建线程

    EventLoop *loop = nullptr;
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock,[this](){
            return loop_ != nullptr;
        });
        loop = loop_;
    }
    return loop;
 }

 //下面这个方法 是在单独的新线程里面运行的
 void EventLoopThread::threadFunc()
 {
    EventLoop loop;  //创建一个独立的EventLoop对象 和上面的线程是一一对应的 one loop per thread

    if(callback_)
    {
        callback_(&loop);
    }

    {
        std::unique_lock<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }
    
    loop.loop() ; //EventLoop的loop() 开启了底层的Poller的epoll()
    std::unique_lock<std::mutex> lock(mutex_);
    loop_ = nullptr;
 }