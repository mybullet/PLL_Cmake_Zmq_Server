
#ifndef ZMQSERVER_H
#define ZMQSERVER_H

#include <QThread>
#include <QDebug>
#include <QObject>

#include "zmq.hpp"
#include <chrono>
#include <openssl/rand.h>
#include <openssl/evp.h>


class ZmqServer : public QThread
{
    Q_OBJECT
public:
    explicit ZmqServer(QObject *parent = nullptr);

    void init(std::string replyer_connect_str, std::string publisher_connect_str);

    void run() override;

    void stop();

 private:
    std::string generateRandomKey(int keyLength);
    std::string encrypt(const std::string& message, const std::string& key);
    std::string decrypt(const std::string& encryptedMessage, const std::string& key);
    std::string GetRealtimeStr();
    void CreateIpcFile();

signals:
    void SignalReceivedMsg(std::string msg);


private:
    std::string m_replyer_connect_str;
    std::string m_publisher_connect_str;
    bool m_is_running = false;
};

#endif // ZMQSERVER_H
