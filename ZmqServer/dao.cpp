#include "dao.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

void Dao::SlotInsertMsg(std::string msg)
{
    try
    {
        static QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
        db.setHostName("127.0.0.1");
        db.setPort(3306);
        db.setDatabaseName(m_data_source.c_str());
        db.setUserName(m_user_name.c_str());
        db.setPassword(m_password.c_str());
        bool ok = db.open();
        if (ok)
        {
            if(!CheckTableExist(db, GetRealTimeTableName()))
            {
                qDebug() << "table not exist and create";
                emit SignalCreateNewTable(db, GetRealTimeTableName());
            }

            //将msg插入到GetRealTimeTableName()表格中
            QSqlQuery query(db);
            std::string sql = "insert into " + GetRealTimeTableName() + " (msg) values ('" + msg + "')";
            if(!query.exec(sql.c_str()))
            {
                qDebug() << "insert failed : " << query.lastError().text();
            }

            qDebug() << "insert msg finished";

            db.close();
        }
        else
        {
            qDebug() << "faile to open db";

        }
    }
    catch(const std::exception& e)
    {
        qDebug() << "eeeeeeeeeee"<< e.what() << '\n';
    }
       
}

bool Dao::CheckTableExist(QSqlDatabase& db, std::string table_name)
{
    try
    {
        QSqlQuery query(db);
        std::string sql = "select count(*) from information_schema.TABLES where table_name = '" + table_name + "'";
        if(!query.exec(sql.c_str()))
        {
            qDebug() << "CheckTableExist failed : " << query.lastError().text();
        }
        query.next();
        int count = query.value(0).toInt();
        if (count > 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    catch(const std::exception& e)
    {
        qDebug() << "sdsds" << e.what() << '\n';
        return false;
    }
    
}

std::string Dao::GetRealTimeTableName()
{
    time_t now = time(0);
    tm* ltm = localtime(&now);
    std::string year = std::to_string(1900 + ltm->tm_year);
    std::string month = std::to_string(1 + ltm->tm_mon);
    std::string day = std::to_string(ltm->tm_mday);
    std::string real_time_table_name = "table_" +  year + "_" + month + "_" + day;
    return real_time_table_name;
}

void Dao::SlotCreateNewTable(QSqlDatabase& db, std::string table_name)
{
    try
    {
        QSqlQuery query(db);
        std::string sql = "create table " + table_name + " (dt datetime(3) primary key, msg varchar(255))";
        if (!query.exec(sql.c_str()))
        {
            qDebug() << "create table failed: " << query.lastError().text();
        }

        // 添加触发器：当插入新数据时，dt列默认值为当前时间戳（精确到毫秒）
        sql = "CREATE TRIGGER " + table_name + "_insert_value_trigger BEFORE INSERT ON " + table_name + " FOR EACH ROW SET NEW.dt = DATE_FORMAT(NOW(3), '%Y-%m-%d %H:%i:%s.%f')";
        if (!query.exec(sql.c_str()))
        {
            qDebug() << "Create trigger failed: " << query.lastError().text();
        }



        //添加触发器：当插入新数据时，msg列的最短字符长度为5
        sql = "create trigger " + table_name + "_insert_check_trigger before insert on " + table_name + " for each row if length(new.msg) < 5 then signal sqlstate '45000' set message_text = 'message length must be greater than 5'; end if";
        if(!query.exec(sql.c_str()))
        {
            qDebug() << "create trigger failed : " << query.lastError().text();
        }

        //添加触发器：当更新数据时，msg列的最短字符长度为5
        sql = "create trigger " + table_name + "_update_check_trigger before update on " + table_name + " for each row if length(new.msg) < 5 then signal sqlstate '45000' set message_text = 'message length must be greater than 5'; end if";
        if(!query.exec(sql.c_str()))
        {
            qDebug() << "create trigger failed : " << query.lastError().text();
        }

        qDebug() << "SlotCreateNewTable finished";
    }
    catch(const std::exception& e)
    {
        qDebug() << "eeeeeeeeeerr" << e.what() << '\n';
    }
    
    
}
