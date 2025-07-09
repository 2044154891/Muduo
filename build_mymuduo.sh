#!/bin/bash
set -e

# 1. 创建 build 目录
if [ ! -d "$(pwd)/build" ]; then
    mkdir "$(pwd)/build"
fi

# 2. 清空 build 目录
rm -rf "$(pwd)/build/*"

# 3. 进入 build 目录并构建
cd "$(pwd)/build"
cmake ..
make

# 4. 回到项目根目录
cd ..

# 5. 创建 /usr/include/Muduo 目录
if [ ! -d "/usr/include/Muduo" ]; then
    sudo mkdir /usr/include/Muduo
fi

# 6. 复制所有头文件到 /usr/include/Muduo
sudo cp -r include/* /usr/include/Muduo/

# 7. 复制 libMuduo.so 到 /usr/lib
sudo cp lib/libMuduo.so /usr/lib/

echo "[Muduo] 构建完成：头文件已安装到 /usr/include/Muduo，库文件已安装到 /usr/lib。" 