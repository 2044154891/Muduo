# Muduo 网络库

## 项目简介

Muduo 是一个高性能、易用的 C++ 网络库，支持多线程 Reactor 模型，适合用于高并发服务器开发。该项目实现了 TCP 服务器、事件循环、线程池、日志等基础网络编程组件，接口简洁，易于扩展。

## 主要功能
- 支持多线程高并发 TCP 服务器开发
- Reactor 事件驱动模型
- 线程池自动分发连接
- 统一的回调接口（连接、消息、写完成）
- 日志、缓冲区、定时器等常用网络编程工具

## 依赖环境
- Linux (建议 Ubuntu 20.04 及以上)
- g++ (支持 C++11)
- cmake
- make
- pthread

## 项目拉取与构建

1. **克隆项目**
   ```bash
   git clone <你的仓库地址> Muduo
   cd Muduo
   ```

2. **一键构建与安装头文件/库文件**
   
   项目根目录下已提供自动化脚本：
   ```bash
   sudo bash build_mymuduo.sh
   ```
   - 会自动创建 build 目录并编译
   - 头文件会被安装到 `/usr/include/Muduo`
   - 动态库 `libMuduo.so` 会被安装到 `/usr/lib`

## 示例代码编译与运行

1. **进入 example 目录**
   ```bash
   cd example
   ```
2. **编译示例 echo_server**
   ```bash
   make
   ```
   - 会自动编译生成 `echo_server` 可执行文件

3. **运行 echo_server**
   ```bash
   ./echo_server
   ```
   - 服务器会监听 12345 端口，支持多线程回显

4. **用 nc 或 telnet 测试**
   ```bash
   nc 127.0.0.1 12345
   # 或
   telnet 127.0.0.1 12345
   ```
   - 输入任意内容回车，服务器会原样返回

## 典型用法说明

- 只需包含头文件：
  ```cpp
  #include <Muduo/TcpServer.h>
  ```
- 继承或组合 TcpServer，设置回调：
  ```cpp
  server.setConnectionCallback(...);
  server.setMessageCallback(...);
  server.setWriteCompleteCallback(...);
  ```
- 支持多线程：
  ```cpp
  server.setThreadNum(4); // 设置4个工作线程
  ```
- 事件循环启动：
  ```cpp
  server.start();
  loop.loop();
  ```

## 目录结构说明
- `include/` 头文件
- `src/`     源码实现
- `lib/`     编译生成的动态库
- `build/`   构建中间文件
- `example/` 示例代码与 Makefile

## 常见问题
- **找不到头文件/库**：请确保已执行 `sudo bash build_mymuduo.sh` 并已安装到系统目录。
- **std::bad_function_call**：请确保所有回调都已设置，且 `TcpServer` 回调已正确传递给 `TcpConnection`。

---

如有问题欢迎提 issue 或联系作者！ 