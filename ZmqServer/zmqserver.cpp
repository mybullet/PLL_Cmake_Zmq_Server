
#include "zmqserver.h"
#include <QDir>
#include <fstream>
#include <iostream>
#include <cstdlib>

ZmqServer::ZmqServer(QObject *parent)
    : QThread{parent}
{

}

void ZmqServer::init(std::string replyer_connect_str, std::string publisher_connect_str)
{

    //m_publisher_connect_str = "ipc://" + CreateIpcFile();
    //std::cout<< "ipc con str : " << m_publisher_connect_str;
    m_replyer_connect_str = replyer_connect_str;
    m_publisher_connect_str = publisher_connect_str;
    m_is_running = true;
}

std::string ZmqServer::CreateIpcFile()
{
    QDir dir;

    const char* user = std::getenv("USER");
    std::string path = "/tmp";
    // if (user != nullptr) {
    //     path = "/home/" + std::string(user) + "/tmp";
        

    //     if (!dir.exists(QString::fromStdString(path)))   
    //     {
    //         dir.mkdir(QString::fromStdString(path));
    //     }

    // } else {
    //     std::cerr << "Failed to get user" << std::endl;
        
    //     if (!dir.exists("../../../../tmp"))
    //     {
    //         dir.mkdir("../../../../tmp");
    //     }
    //     path = "../../../../tmp";
    // }

    path += "/test.ipc";
    std::ofstream file(path);
    file.close();

    std::cout << "ipc file path : "<< path << std::endl;

    return path;
}

void ZmqServer::run()
{
    zmq::context_t context(2);
    zmq::socket_t replyer(context, zmq::socket_type::rep);
    zmq::socket_t publisher(context, zmq::socket_type::pub);

    try
    {
        int keyLength = 32; // 32 字节
        std::string key = generateRandomKey(keyLength);

        std::cout << QString::fromStdString("new generated key : " + key).toStdString();


        replyer.bind(m_replyer_connect_str.c_str());
        std::cout << "finish bind reply";

        std::cout << m_publisher_connect_str.c_str();
        publisher.bind(m_publisher_connect_str.c_str());
        std::cout << "finish bind publish";

        zmq::message_t request;
        std::string reply;

        while (m_is_running)
        {

            // 接收来自客户端的请求
            replyer.recv(request, zmq::recv_flags::none);
            std::string message(static_cast<char*>(request.data()), request.size());

            // 检查消息前缀
            if (message.find("GET@") == 0)
            {
                reply = "KEY@" + key;

                zmq::message_t response(reply.size());
                memcpy(response.data(), reply.data(), reply.size());
                replyer.send(response, zmq::send_flags::none);
            }
            else
            {
                std::cout << QString::fromStdString("Received origin message from working-client: " + message).toStdString();
                message = decrypt(message, key);
                if (message.find("LOCAL@") == 0)
                {
                    reply = "REMOTE@ hello, client!";
                    reply = encrypt(reply, key);
                    zmq::message_t response(reply.size());
                    memcpy(response.data(), reply.data(), reply.size());
                    replyer.send(response, zmq::send_flags::none);

                    //send message to http server
                    std::string http_message_str = GetRealtimeStr() + "  " + message.substr(6);
                    zmq::message_t http_message(http_message_str.size());
                    memcpy(http_message.data(), http_message_str.c_str(), http_message_str.size());
                    publisher.send(zmq::str_buffer("STATE"), zmq::send_flags::sndmore);
                    publisher.send(http_message, zmq::send_flags::none);

                    emit SignalReceivedMsg(http_message_str);

                    std::cout << QString::fromStdString("Send Message to Http server: " + http_message_str).toStdString();
                }
                else
                {
                    std::cout << QString::fromStdString("Received invalid message from working-client: " + message).toStdString();
                    continue;
                }
            }


            std::cout << QString::fromStdString("Received message from working-client: " + message).toStdString();

            QThread::msleep(250);
        }

    }
    catch(const std::exception& e)
    {
        std::cout << "gggggggggg"<< e.what() << '\n';
    }

    replyer.close();
    publisher.close();
    context.shutdown();
}

void ZmqServer::stop()
{
    m_is_running = false;
}

std::string ZmqServer::generateRandomKey(int keyLength)
{
    unsigned char* key = new unsigned char[keyLength];
    RAND_bytes(key, keyLength);
    std::string keyString(reinterpret_cast<char*>(key), keyLength);
    delete[] key;
    return keyString;
}

std::string ZmqServer::encrypt(const std::string& message, const std::string& key)
{
    // 创建加密上下文
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char*>(key.c_str()), NULL);

    // 加密数据
    int cipherLen = message.length() + EVP_MAX_BLOCK_LENGTH;
    unsigned char* cipherText = new unsigned char[cipherLen];
    int len;
    EVP_EncryptUpdate(ctx, cipherText, &len, reinterpret_cast<const unsigned char*>(message.c_str()), message.length());
    cipherLen = len;
    EVP_EncryptFinal_ex(ctx, cipherText + len, &len);
    cipherLen += len;

    // 清理并释放资源
    EVP_CIPHER_CTX_free(ctx);

    // 返回加密后的数据
    std::string encryptedText(reinterpret_cast<char*>(cipherText), cipherLen);
    delete[] cipherText;
    return encryptedText;
}

std::string ZmqServer::decrypt(const std::string& encryptedMessage, const std::string& key)
{
    // 创建解密上下文
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char*>(key.c_str()), NULL);

    // 解密数据
    int plainLen = encryptedMessage.length();
    unsigned char* plainText = new unsigned char[plainLen];
    int len;
    EVP_DecryptUpdate(ctx, plainText, &len, reinterpret_cast<const unsigned char*>(encryptedMessage.c_str()), encryptedMessage.length());
    plainLen = len;
    EVP_DecryptFinal_ex(ctx, plainText + len, &len);
    plainLen += len;

    // 清理并释放资源
    EVP_CIPHER_CTX_free(ctx);

    // 返回解密后的数据
    std::string decryptedText(reinterpret_cast<char*>(plainText), plainLen);
    delete[] plainText;
    return decryptedText;
}

std::string ZmqServer::GetRealtimeStr()
{
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    // convert timestamp to string format
    std::time_t t = timestamp / 1000;
    std::tm tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << timestamp % 1000;
    std::string timestamp_str = oss.str();
    return timestamp_str;
}

