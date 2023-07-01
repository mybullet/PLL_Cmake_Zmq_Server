
#include <QCoreApplication>

#include <iostream>
#include <chrono>
#include <thread>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <fstream>
#include <QSqlDatabase>
#include "dao.h"
#include <QThread>
#include <QDebug>
#include <QObject>
#include <QtConcurrent/QtConcurrent>
#include "zmqserver.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    Dao dao("data_source_0625");
    QThread thread;
    dao.moveToThread(&thread);

    //QObject::connect(&thread, SIGNAL(started()), &dao, SLOT(SlotInsertMsg()));

    thread.start();
    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&]()
    {
        thread.quit();
        thread.wait();
    });


    ZmqServer zmq_server;
    zmq_server.init("tcp://192.168.3.5:7777", "ipc://./tmp/test.ipc");

    zmq_server.start();

    QObject::connect(&app, &QCoreApplication::aboutToQuit, [&]()
    {
        zmq_server.stop();
        thread.quit();
        thread.wait();
    });

    QObject::connect(&zmq_server, SIGNAL(SignalReceivedMsg(std::string)), &dao, SLOT(SlotInsertMsg(std::string)));


    // 使用QtConcurrent运行函数在子线程中
    //QFuture<void> future = QtConcurrent::run(RunZmqServer);
    
    // 等待子线程完成
    //future.waitForFinished();

    return app.exec();
}
