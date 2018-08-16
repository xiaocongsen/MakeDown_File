# 安装
## mac
    先装boost
    brew install mysql-connector-c++
## ubuntu 
    //sudo apt-get install libmysqlclient-dev
    sudo apt-get install libmysqlcppconn-dev
   
## Centos
    先决条件 本机上装的mysql不能低于5.7
    https://github.com/mysql/mysql-connector-cpp
    选择1.1.11版本
    下载或者clone
    yum install mysql-devel
    安装 boost 库
    wget https://dl.bintray.com/boostorg/release/1.67.0/source/boost_1_67_0.tar.gz
    解压 执行 ./bootstrap.sh --prefix=/usr/local/include/boost
    ./b2 install
    

# Mysql连接池例子
## 连接池.h文件
```c++
#ifndef _DB_POOL_H_  
#define _BD_POOL_H_  
  
#include <iostream>  
#include <mysql_connection.h>    
#include <mysql_driver.h>    
#include <cppconn/exception.h>    
#include <cppconn/driver.h>    
#include <cppconn/connection.h>    
#include <cppconn/resultset.h>    
#include <cppconn/prepared_statement.h>    
#include <cppconn/statement.h>    
#include <pthread.h>    
#include <list>    
  
using namespace std;
using namespace sql;
  
class DBPool  
{  
public:  
    // Singleton   
    static DBPool& GetInstance();  
  
    //init pool  
    void initPool(std::string url_, std::string user_, std::string password_, int maxSize_);  
  
    //get a conn from pool  
    Connection* GetConnection();  
  
    //put the conn back to pool  
    void ReleaseConnection(Connection *conn);  
  
    ~DBPool();  
  
private:  
    DBPool(){}  
  
    //init DB pool  
    void InitConnection(int initSize);  
      
    // create a connection  
    Connection* CreateConnection();   
  
    //destory connection  
    void DestoryConnection(Connection *conn);  
  
    //destory db pool  
    void DestoryConnPool();  
  
private:  
    string user;  
    string password;  
    string url;  
    int maxSize;  
    int curSize;  
  
    Driver*             driver;     //sql driver (the sql will free it)  
    list<Connection*>   connList;   //create conn list  
  
    //thread lock mutex  
    static pthread_mutex_t lock;       
};  
  
#endif  
```

## 连接池.cpp文件
```c++
#include <iostream>  
#include <stdexcept>    
#include <exception>    
#include <stdio.h>    
#include "DBPool.h"  
  
using namespace std;  
using namespace sql;  
  
pthread_mutex_t DBPool::lock = PTHREAD_MUTEX_INITIALIZER;  
  
//Singleton: get the single object  
DBPool& DBPool::GetInstance()  
{    
    static DBPool instance_;  
    return instance_;    
}  
  
void DBPool::initPool(std::string url_, std::string user_, std::string password_, int maxSize_)  
{    
    this->user = user_;    
    this->password = password_;  
    this->url = url_;  
    this->maxSize = maxSize_;    
    this->curSize = 0;   
  
    try{    
        this->driver=sql::mysql::get_driver_instance();    
    }    
    catch(sql::SQLException& e)    
    {    
        perror("Get sql driver failed");    
    }    
    catch(std::runtime_error& e)    
    {    
        perror("Run error");    
    }    
    this->InitConnection(maxSize/2);  
}   
  
//init conn pool  
void DBPool::InitConnection(int initSize)  
{  
    Connection* conn;    
    pthread_mutex_lock(&lock);    
    for(int i =0;i <initSize; i++)    
    {    
        conn= this->CreateConnection();    
          
        if(conn)  
        {    
            connList.push_back(conn);    
            ++(this->curSize);    
        }    
        else    
        {    
            perror("create conn error");    
        }    
    }    
    pthread_mutex_unlock(&lock);    
  
}  
  
Connection* DBPool::CreateConnection()  
{  
    Connection* conn;    
    try{    
        conn = driver->connect(this->url,this->user,this->password);  //create a conn   
        return conn;    
    }    
    catch(sql::SQLException& e)    
    {    
        perror("link error");    
        return NULL;    
    }    
    catch(std::runtime_error& e)    
    {    
        perror("run error");    
        return NULL;    
    }    
}  
  
Connection* DBPool::GetConnection()  
{    
    Connection* conn;    
  
    pthread_mutex_lock(&lock);  
  
    if(connList.size()>0)//the pool have a conn   
    {    
        conn = connList.front();   
        connList.pop_front();//move the first conn   
        if(conn->isClosed())//if the conn is closed, delete it and recreate it  
        {    
            delete conn;    
            conn = this->CreateConnection();    
        }    
  
        if(conn == NULL)    
        {    
            --curSize;    
        }  
  
        pthread_mutex_unlock(&lock);  
  
        return conn;    
    }    
    else  
    {    
        if(curSize< maxSize)//the pool no conn  
        {  
            conn = this->CreateConnection();    
            if(conn)  
            {    
                ++curSize;    
                pthread_mutex_unlock(&lock);    
                return conn;    
            }    
            else  
            {    
                pthread_mutex_unlock(&lock);    
                return NULL;    
            }    
        }    
        else //the conn count > maxSize  
        {   
            pthread_mutex_unlock(&lock);    
            return NULL;    
        }    
    }    
}    
  
//put conn back to pool  
void DBPool::ReleaseConnection(Connection *conn)  
{  
    if(conn)  
    {    
        pthread_mutex_lock(&lock);    
        connList.push_back(conn);    
        pthread_mutex_unlock(&lock);    
    }    
}  
  
void DBPool::DestoryConnPool()  
{    
    list<Connection*>::iterator iter;    
    pthread_mutex_lock(&lock);    
    for(iter = connList.begin(); iter!=connList.end(); ++iter)    
    {    
        this->DestoryConnection(*iter);   
    }    
    curSize=0;    
    connList.clear();  
    pthread_mutex_unlock(&lock);    
}    
  
   
void DBPool::DestoryConnection(Connection* conn)    
{    
    if(conn)    
    {    
        try{    
            conn->close();    
        }    
        catch(sql::SQLException&e)    
        {    
            perror(e.what());    
        }    
        catch(std::exception& e)    
        {    
            perror(e.what());    
        }   
        delete conn;    
    }    
}   
   
DBPool::~DBPool()    
{    
    this->DestoryConnPool();    
}  
```
## main.cpp文件
```c++
#include "DBPool.h"  
#include <stdio.h>   
  
/*-------------------------------------------------------------- 
    单例模式，全局唯一 db pool，程序中使用onnpool中获取一个 
    db连接使用，使用完之后调用ReleaseConnection把conn放回pool中去. 
----------------------------------------------------------------*/  
DBPool  connpool = DBPool::GetInstance();  
  
int main(int argc, char* argv[])   
{  
    //初始化连接，创建参数中maxSize一半的连接  
    connpool.initPool("tcp://127.0.0.1:3306", "root", "123456", 100);  
  
    Connection *con;  
    Statement *state;  
    ResultSet *result;  
    con = connpool.GetConnection();//get a db conn  
    for(int i = 0; i<1000; i++)  
    {  
        state = con->createStatement();  
        state->execute("use mysql");  
   
        // 查询  
        result = state->executeQuery("select host,user from user");  
   
        // 输出查询  
        while (result->next())   
        {  
            try{  
                string user = result->getString("user");   
                string name = result->getString("host");  
                cout << user << " : " << name << endl;  
            }catch(sql::SQLException& e){  
                std::cout << e.what() << std::endl;  
            }  
        }  
  
        /*result = state->executeQuery("select cust_id,cust_name from customers"); 
        while (result->next())  
        { 
            try{ 
                string user = result->getString("cust_id"); 
                string name = result->getString("cust_name"); 
                cout << user << " : " << name << endl; 
            }catch(sql::SQLException& e){ 
              std::cout << e.what() << std::endl; 
            } 
        } 
    */  
        std::cout << i << std::endl;  
      
    }
    //执行存储过程
    {
        std::auto_ptr<sql::Statement> stmt(con->createStatement()); //使用智能指针
        char buff[256];
        const char * strEntrustSql = "CALL EntrustProcess('%s','%s',%s,%s,%s,%s,%s,%s,%s,%s,%s,%d,%d,@pop)";
        sprintf(buff,strEntrustSql,"2015052212425","20150522150753","100.000012","10.000045","23.423421","3453","564.534","5435.54","234","434.34","743.345",3,3);
        stmt->execute(buff);
        std::auto_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT @pop AS _reply")); //使用智能指针
        while (res->next());
        std::cout << "Population of Uganda: " << res->getString("_reply") << std::endl;
    }
    
    delete result;  
    delete state;  
    connpool.ReleaseConnection(con);  
  
    return 0;  
}  
```


```c++
    ResultSet *rs
    rs->rowsCount();   //获得行数
    ResultSetMetaData *res_meta = rs->getMetaData();
    int numcols = res_meta->getColumnCount();  //获得列数
    cout.width(20);
    cout << "Column Name/Label";    //列名
    cout.width(20); 
    cout << "Column Type";          //列字段类型
    cout.width(20);
    cout << "Column Size" << endl;  //列字段大小

    for (int i = 0; i < numcols; ++i)
    {
        cout.width(20);
        cout << res_meta->getColumnLabel(i + 1);                //获第i列字段名
        cout.width(20);
        cout << res_meta->getColumnTypeName(i + 1);             //获第i列字段类型
        cout.width(20);
        cout << res_meta->getColumnDisplaySize(i + 1) << endl   //获第i列字段大小
        << endl;
    }
    res_meta->getTableName(1);      //表名
    res_meta->getSchemaName(1);     //数据库名
```