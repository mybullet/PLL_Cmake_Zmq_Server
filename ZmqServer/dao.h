#pragma once
#include <QObject>
#include <QThread>
#include <QDebug>
#include <QSqlDatabase>

class Dao : public QObject
{
    Q_OBJECT
public:
    explicit Dao(std::string data_source, std::string user_name = "root", std::string password = "qwer1234", QObject *parent = nullptr) 
    : m_data_source(data_source), m_user_name(user_name), m_password(password), QObject(parent)
    {
        connect(this, SIGNAL(SignalCreateNewTable(QSqlDatabase&, std::string)), this, SLOT(SlotCreateNewTable(QSqlDatabase&, std::string)));

    }

    bool CheckTableExist(QSqlDatabase& db, std::string table_name);
    std::string GetRealTimeTableName();

public slots:
    void SlotInsertMsg(std::string msg = "hello, world!");
    void SlotCreateNewTable(QSqlDatabase& db, std::string table_name);
signals:
    void SignalCreateNewTable(QSqlDatabase& db, std::string table_name);

private:
    std::string m_data_source;
    std::string m_user_name;
    std::string m_password;
};
