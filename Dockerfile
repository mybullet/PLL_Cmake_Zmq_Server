# 使用的基础镜像
FROM ubuntu:latest

RUN apt-get update && apt-get install -y apt-utils

# 安装所需的软件包和依赖项
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    qt5-qmake \
    qtbase5-dev \
    libssl-dev \
    libzmq3-dev \
    libmysqlclient-dev \
    libqt5sql5-mysql \
    iputils-ping \
    netcat



EXPOSE 7777
EXPOSE 3306
# 复制应用程序源代码到镜像中
COPY ./ZmqServer /app/ZmqServer

# WORKDIR /app
# RUN mkdir tmp && cd /app/tmp && touch test.ipc && chmod 777 test.ipc
# ENV ZMQ_IPC_FILE /app/tmp/test.ipc

WORKDIR /app/ZmqServer/build
RUN cmake .. && make

# 设置工作目录
WORKDIR /app/ZmqServer/build

# 定义容器启动时执行的命令
CMD ["./start_zmq_server.sh"]
