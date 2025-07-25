#include <functional>
#include <string.h>

#include "TcpServer.h"

static EventLoop *CheckLoopNotNull(EventLoop *loop)
{
    if(loop == nullptr)
    {
        LOG_FATAL("%s:%s:%d mainLoop is null!\n",__FILE__, __FUNCTION__, __LINE__);
    }
    return loop;
}

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr, const std::string &nameArg, Option option)
    : loop_(CheckLoopNotNull(loop))
    , ipPort_(listenAddr.toIpPort())
    , name_(nameArg)
    , acceptor_(new Acceptor(loop, listenAddr, option == kReusePort))
    , threadPool_(new EventLoopThreadPool(loop, name_))
    , connectionCallback_()
    , messageCallback_()
    , nextConnId_(1)
    , started_(0)

{
    //当有新用户连接时，Acceptor类中绑定的acceptChannel_会有读事件发送，执行handleRead()调用TcpServer::newConnection回调
    acceptor_->setNewConnectionCallback(
        std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{
    for(auto &item : connections_)
    {
        TcpConnectionPtr conn(item.second);
        item.second.reset(); //把原始的智能指针复位 栈空间的TcpConnectionPtr conn指向该对象 当conn出了其作用域 即可释放智能指针对象
    }
}

// 设置底层的subloop的个数
void TcpServer::setThreadNum(int numThreads)
{
    int numThreads_ = numThreads;
    threadPool_->setThreadNum(numThreads_);
}

//开启服务器监听
void TcpServer::start()
{
    if(started_.fetch_add(1) == 0) //防止一个TcpServer对象被start多次
    {
        threadPool_->start(threadInitCallback_);  //启动底层的loop线程池
        loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
    }
}

//有一个新用户连接，acceptor会执行这个回调操作，负责将mainLoop接收到的请求连接(acceptchannel_会有读事件发生)通过回调轮询分发给subloop去处理
void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr)
{
    //轮询算法 选择一个subloop来管理connfd对应的channel
    EventLoop * ioLoop = threadPool_->getNextLoop();
    char buf[64] = {0};
    snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
    ++nextConnId_; //这里没有设置为原子类型是因为只在mainloop中执行 不涉及线程安全问题
    std::string connName = name_ + buf;

    LOG_INFO("TcpServer::newConnection [%s] -new connection [%s] from %s \n",
                name_.c_str(),connName.c_str(), peerAddr.toIpPort().c_str());
    
    //通过sockfd获取其绑定的本地的ip地址和端口信息
    sockaddr_in local;
    ::memset(&local, 0 ,sizeof(local));
    socklen_t addrlen = sizeof(local);
    ::memset(&addrlen, 0, sizeof(addrlen));
    if(::getsockname(sockfd, (sockaddr *)&local, &addrlen) < 0)
    {
        LOG_ERROR("sockets::getLocalAddr\n");
    }
    InetAddress localAddr(local);
    TcpConnectionPtr conn(new TcpConnection(
        ioLoop,
        connName,
        sockfd,
        localAddr,
        peerAddr
    ));
    connections_[connName] = conn;

    // 关键：把用户设置的回调传递给TcpConnection
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));

    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn)
{
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn)
{
    LOG_INFO("TcpServer::removeConnectionInLoop [%s] - connection %s\n",
        name_.c_str(), conn->name().c_str());
    connections_.erase(conn->name());
    EventLoop * ioloop = conn->getLoop();
    ioloop->queueInLoop(std::bind(&TcpConnection::connectDestory, conn));
}