
# RabbitMq
## Mac 安装启动
  brew install rabbitmq
  /usr/local/Cellar/rabbitmq/3.7.4/sbin/rabbitmq-server   启动
  
## Mac 安装启动
  yum install boost boost-devel boost-doc openssl-devel
  
## Docker安装
    docker run -it ubuntu /bin/bash     安装ubuntu最新版本并进入命令行
    apt-get update
    apt-get upgrade
    apt-get install rabbitmq-server     安装rabbitmq服务器
    rabbitmq-plugins enable rabbitmq_management 安装插件
    service rabbitmq-server start   启动服务
    curl 127.0.0.1:15672    可以查看启动是否成功

    C++需要安装库rabbimq-c SimpleAmqpClient
    apt-get install  net-tools git libboost-dev openssl  libssl-dev cmake vim autoconf automake libtool libffi-dev doxygen graphviz libpopt-dev xmlto
    git clone https://github.com/alanxz/rabbitmq-c   
    mkdir build && cd build
    cmake ..
    cmake --build .
    make
    make install

    git clone https://github.com/alanxz/SimpleAmqpClient  //进入clone的目录
    mkdir simpleamqpclient-build
    cd simpleamqpclient-build
    apt-get install libboost-all-dev  (为了解决找不到boost头文件的问题)
    cmake ..
    make
    make install

    ln -s /usr/local/lib/libSimpleAmqpClient.so.2 /usr/lib/
    ln -s /usr/local/lib/x86_64-linux-gnu/librabbitmq.so.4 /usr/lib/

    将libSimpleAmqpClient.so加入到链接库
    编译程序的时候请用  g++ xxx.cpp -o xxx -lSimpleAmqpClient

``` c++
SimpleAmqpClient中channel的构造函数是这样的:

static ptr_t AmqpClient::Channel::Create(const std::string & host = "127.0.0.1",
                                         int port = 5672,
                                         const std::string & username = "guest",
                                         const std::string & password = "guest",
                                         const std::string & vhost = "/",
                                         int frame_max = 131072 
                                         ) 
```
    rabbitmqctl add_user hello 123456   创建一个用户
    rabbitmqctl set_permissions -p / hello ".*" ".*" ".*"   为用户提供访问vhost的权利

- AMQP协议中的元素包括：Message（消息体）、Producer（消息生产者）、Consumer（消息消费者）、Virtual Host（虚拟节点）、Exchange（交换机）、Queue（队列）等
1. Exchange
    
        生产者只能将消息发送到交换器,交换器 一方面，它接收来自生产者的消息，另一方面，它会将消息推送到队列中。交换器必须知道如何处理接收到的消息：它是否应该附加到某个特定的队列？它应该被附加到许多队列吗？或者应该被丢弃。该规则由交换器的类型定义。可用的交换类型direct, topic, headers 和 fanout。
- fanout 它只会将接收到的所有消息广播发送到它所知道的所有队列。

## 一. 程序用来发送和接收来自一个已命名队列（queue）的消息
- send.cpp代码
``` c++
#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
using namespace std;
int main() 
{
    string queue = "hello";
    /*
    static ptr_t Create(const std::string &host = "127.0.0.1", int port = 5672,
                      const std::string &username = "guest",
                      const std::string &password = "guest",
                      const std::string &vhost = "/", int frame_max = 131072)
    host AMQP代理的主机名或IP地址
    port 用于连接到AMQP代理的端口
    vhost 我们应该连接的AMQP上的虚拟主机
    channel_max请求服务器将此连接的通道数量限制为指定的参数，值为零将使用代理提供的值
    frame_max 请求服务器限制任何的最大大小
    返回一个新的Channel对象指针
    */
    AmqpClient::Channel::ptr_t channel = AmqpClient::Channel::Create("172.17.0.2", 5672, "hello", "123456");

    /*
        std::string DeclareQueue(const std::string &queue_name, bool passive = false, bool durable = false, bool exclusive = true, bool auto_delete = true);
        passive 默认false如果队列不存在 就创建 （如果为true 队列不存在borker就返回错误会回应带有错误和不创建队列）
        durable 是否持久化
        exclusive 表示只有客户端可以使用队列。默认为true。一个当连接关闭时，独占队列被删除
        auto_delete 队列在至少一次交换后将被删除已经被绑定到它，然后被解除约束
        返回在broker上创建的队列的名称。大多数时候使用broker被要求通过不提供队列名称创建一个唯一的队列
    */
    channel->DeclareQueue(queue, false, true, false, false); //队列不存在就创建一个 持久的、 独占的、 自动删除的队列， 该队列名称:queue
    for(int i=0; i < 100000; i++)
    {
            string str="hello";
            char buf[100] = {0};
            sprintf(buf, "%s%d", str.c_str(), i);
            channel->BasicPublish("", queue, AmqpClient::BasicMessage::Create(buf));
    }
    cout << "sent hello";
}
```
- recv.cpp代码 消费者
``` c++
#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <iostream>
#include <string>
using namespace std;
int main() 
{
  while(1) 
  {
    string queue = "hello";
    AmqpClient::Channel::ptr_t channel = AmqpClient::Channel::Create("172.17.0.2", 5672, "hello", "123456");
    channel->DeclareQueue(queue, false, true, false, false);
    AmqpClient::Envelope::ptr_t envelope;
    channel->BasicConsume(queue, "", true, true, false);
    bool success = channel->BasicConsumeMessage(envelope, -1);
    string buffer = envelope->Message()->Body();
    cout << buffer << endl;
  }
}
```

- Send.cpp 2代码如下：
```c++
#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <iostream>
int main() 
{
  std::string queue_name = "hello";     //给队列queue命名
  AmqpClient::Channel::ptr_t channel = AmqpClient::Channel::Create("localhost");    //创建channel //创建到服务器的连接
  channel->DeclareQueue(queue_name, false, true, false, false); //创建队列，第一个参数为队列名称，//第三个参数表明队列是持久的（durable ）。其余后续会提到。
  std::string message;
  std::cin >> message;
  channel->BasicPublish("", queue_name, AmqpClient::BasicMessage::Create(message)); //第一个是exchange名称，第二个参数是routing_key（此处可理解为消息会被送往的队列）。
  std::cout << "[x] send " << message << std::endl;
}
```
- Recv.cpp 2代码如下：
```c++
#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <iostream>

int main() 
{
  std::string queue_name = "hello";     //给队列queue命名
  AmqpClient::Channel::ptr_t channel = AmqpClient::Channel::Create("localhost");    //创建到服务器的连接
  channel->DeclareQueue(queue_name, false, true, false, false);
  std::string consumer_tag = channel->BasicConsume(queue_name, "");
  //第二个参数为消费者名称，返回值也是消费者名称。
  while (1) 
  {
    std::cout << "[y] wait for the message" << std::endl;
    AmqpClient::Envelope::ptr_t envelope = channel->BasicConsumeMessage(consumer_tag);
    std::string buffer = envelope->Message()->Body();
    //消息放在信封里，需要解析
    std::cout << "[y] receve " << buffer << std::endl;
  }
  channel->BasicCancel(consumer_tag);
  //关闭消费者。
}
```

## 二. 创建一个工作队列,用于在多个worker之间分配耗时的任务
- newTask.cpp代码如下：
``` c++
#include "SimpleAmqpClient/SimpleAmqpClient.h"
#include <iostream>
int main() 
{
  AmqpClient::Channel::ptr_t channel = AmqpClient::Channel::Create("localhost");
  std::string queue_name = "task_queue";
  channel->DeclareQueue(queue_name, false, true, false, false);
  //第三个参数表明队列是持久的（durable ）。
  while (1) 
  {
    std::string message;
    std::cin >> message;
    channel->BasicPublish("", queue_name, AmqpClient::BasicMessage::Create(message));
    //第一个参数是exchange的名字
    std::cout << "[x] send " << message << std::endl;
  }
}
```
- worker.cpp的代码如下：
```c++
#include "SimpleAmqpClient/SimpleAmqpClient.h"
#include <iostream>

void do_work(std::string s) 
{
  int sum = 0;
  for (int i = 0; s[i] != 0; i++)
    if (s[i] == '.')
      sum++;
  sleep(sum);
}
//假装处于忙碌状态，一个'.'工作一秒。（参照官网）
int main() 
{
  AmqpClient::Channel::ptr_t channel = AmqpClient::Channel::Create("localhost");
  std::string queue_name = "task_queue";
  channel->DeclareQueue(queue_name, false, true, false, false); 
  std::string consumer_tag = channel->BasicConsume(queue_name, "", true, true, false, 1);
  //将第4个参数改为true,开启消息确认。
  //将第5个参数改为false,取消独占队列。
  //最后一个参数：同时不超过一个任务。（非平均分配）。
  while (1) 
  {
    AmqpClient::Envelope::ptr_t envelope = channel->BasicConsumeMessage(consumer_tag);
    std::string buffer = envelope->Message()->Body();
    std::cout << "[y] receve " << buffer << std::endl;
    do_work(buffer);
  }
  channel->BasicCancel(consumer_tag);
}
```

## 三. 创建一个工作队列,用于在多个worker之间分配耗时的任务

- EmitLog.cpp的代码如下：
```c++
#include "SimpleAmqpClient/SimpleAmqpClient.h"
#include <iostream>

int main() 
{
  AmqpClient::Channel::ptr_t channel = AmqpClient::Channel::Create("localhost");

  std::string exchange_name = "logs";
  /*
    声明exchange如果AMQP broker尚不存在，则在AMQP broker上创建exchange
    void DeclareExchange(
      const std::string &exchange_name,
      const std::string &exchange_type = Channel::EXCHANGE_TYPE_DIRECT,
      bool passive = false, bool durable = false, bool auto_delete = false);
      exchange_name exchange的name
      exchange_type 要声明的exchange的类型。 默认指定可以使用的其他类型： fanout and topic
      passive 指示如果exchange不存在，broker应该如何反应。 如果被动为真并且不存在，则broker将响应一个错误并且不创建exchange，否则创建exchange。 默认为false（如果exchange尚不存在，则创建exchange）
      durable 指示nexchange是否持久 - 例如，它是否会在broker重新启动时存活默认为false
      auto_delete 指示当没有队列绑定到它时，是否自动删除nexchange。 默认为false
  */
  channel->DeclareExchange(exchange_name, AmqpClient::Channel::EXCHANGE_TYPE_FANOUT);
  //创建一个名为"logs"的交换器，类型为fanout。

  std::string queue_name = channel->DeclareQueue("", false, true, false, false);
  //第一个参数为空，系统默认生成随机队列名称。
  //第三个参数表明队列是持久的（durable ）。

  channel->BindQueue(queue_name, exchange_name, "");
  //将队列与交换器绑定。
  // routingKey的值将被 fanout交换器忽略。

  while (1) 
  {
    std::cout << "请输入要发送的信息：" << std::endl;

    std::string message;
    std::cin >> message;

    channel->BasicPublish(exchange_name, queue_name, AmqpClient::BasicMessage::Create(message));

    std::cout << "[x] send " << message << std::endl;
  }
}
```
- ReceiveLogs.cpp代码如下：
```c++
#include "SimpleAmqpClient/SimpleAmqpClient.h"
#include <iostream>
int main() 
{
  AmqpClient::Channel::ptr_t channel = AmqpClient::Channel::Create("localhost");

  std::string exchange_name = "logs";
  channel->DeclareExchange(exchange_name, AmqpClient::Channel::EXCHANGE_TYPE_FANOUT);
  //创建一个名为"logs"的交换器，类型为fanout。

  std::string queue_name = channel->DeclareQueue("", false, true, false, false);
  //第一个参数为空，系统默认生成随机队列名称。
  //第三个参数表明队列是持久的（durable ）。

  channel->BindQueue(queue_name, exchange_name, "");
  //将队列与交换器绑定。
  // routingKey的值将被 fanout交换器忽略。

  std::string consumer_tag =
      channel->BasicConsume(queue_name, "", true, true, false, 1);
  //将第4个参数改为true,开启消息确认。
  //将第5个参数改为false,取消独占队列。
  //最后一个同时不超过一个任务。（非平均分配）。

  while (1) 
  {
    std::cout << "等待接收信息中" << std::endl;

    AmqpClient::Envelope::ptr_t envelope =
        channel->BasicConsumeMessage(consumer_tag); //消费一条消息

    std::string buffer = envelope->Message()->Body();

    std::cout << "[y] receve " << buffer << std::endl;
  }

  channel->BasicCancel(consumer_tag);   //取消先前创建的消费者  作为消费者取消订阅队列。
}
```

## rabbitmq命令
rabbitmqctl list_users   查看用户列表

添加 Virtual Hosts ：    
rabbitmqctl add_vhost <vhost>    
    
删除 Virtual Hosts ：    
rabbitmqctl delete_vhost <vhost>    
    
添加 Users ：    
rabbitmqctl add_user <username> <password>    
rabbitmqctl set_user_tags <username> <tag> ...  
tag:
1. management ：访问 management plugin；    
2. policymaker ：访问 management plugin 和管理自己 vhosts 的策略和参数；    
3. monitoring ：访问 management plugin 和查看所有配置和通道以及节点信息；    
4. administrator ：一切权限；    
5. None ：无配置  

rabbitmqctl set_permissions [-p <vhost>] <user> <conf> <write> <read>    
    
删除 Users ：    
rabbitmqctl delete_user <username>   

rabbitmqctl add_vhost vh    
rabbitmqctl add_user test test    
rabbitmqctl set_user_tags test management    
rabbitmqctl set_permissions -p vh test ".*" ".*" ".*"   

rabbitmqctl list_exchanges      列出服务器上的交换器    amq.*后缀的交换器和默认(未命名)的交换器