# MongoDB 数据库的基础命令整理
## 简介及安装
MongoDB是一种基于BSON(Binary-JSON)的面向文档的非关系型数据库

## 安装 MongoDB
sudo apt-get install mongodb

## 查看版本号
mongo -version

## 启动和关闭 MongoDB 服务
service mongodb start
service mongodb stop

## 查看 MongoDB 服务是否启动成功
service mongodb status
pgrep mongo -l

## 卸载 MongoDB
sudo apt-get –purge remove mongodb mongodb-clients mongodb-server

## 文档和集合的概念
多个键及其关联的值有序的放在一起就是文档,类似于关系型数据库中的行,文档中不允许有重复的键

{“key1”:”value1”,”key2”:”value2”}

集合就是一组文档,相当于关系型数据库中的表,但是集合是无模式的,不要求文档格式保持一致

{“key1”:”value1”,”key2”:”value2”}
{“key3”:value}

以上两个文档放在一起就是一个集合,同时两个文档的键不同,值的类型也不同,但不推荐这样做,最好是保持一个集合中的文档类型格式一致

## 数据库常用操作
### 显示数据库列表
show dbs;

### 显示当前数据库中的集合
show collections;

### 显示所有用户
show users;

### 查看当前使用的数据库
db;

### 切换当前数据库
use 数据库名;

### 显示数据库操作命令
db.help();

### 显示集合操作命令
db.集合名.help();

### 创建数据库
MongoDB 中没有直接创建数据库的命令,如果想创建一个数据库,可以先切换到该数据库执行一些操作,数据库将会被自动创建
### 删除数据库
use 数据库名;
db.dropDatabase();

### 创建集合
db.createCollection(‘集合名’);
也可以不预先创建集合,直接使用一个不存在的集合,插入数据时会自动创建

### 删除集合
db.集合名.drop();

### 创建用户
db.createUser({user:”用户名”,pwd:”密码”,roles:[{role:”readWrite”,db:”数据库名”}]}); 
use 数据库名;
db.addUser(‘用户名’,’密码’); //此方式被废弃,虽仍然可用,但不建议使用

### 认证用户
db.auth(‘用户名’,’密码’);

### 删除用户
db.dropUser(“用户名”);

### 权限管理
MongoDB 默认不需要用户授权登录,任何人直接就可以使用,但这样不安全,所以需要开启权限管理
步骤如下:
先在 admin 数据库下创建一个管理员用户
use admin;
db.addUser(‘root’,’123456’);

退出 MongoDB,修改配置文件 /etc/mongodb.conf 使用认证登录
sudo vim /etc/mongodb.conf // 把 #auth=true 前面的 # 去掉
service mongodb restart; // 重启 MongoDB 服务

登录 MongoDB,在 admin 数据库下认证管理员用户
mongo;
use admin;
db.auth(‘root’,’123456’); // 返回 1 成功, 0 失败

去对应数据库下创建数据库专属用户并认证
use test;
db.addUser(‘test’,’123456’);
db.auth(‘test’,’123456’);

## 第三方软件连接,使用 test 用户连接 test 数据库
### 对文档的操作
#### 插入文档
db.集合名.insert({_id:1, key1:value1, key2:value2});
db.集合名.save({_id:1, key1:value1, key2:value2});

_id 项为默认主键,可以不显示指定
当不指定 _id 时,两种方式都为插入新文档
当指定 _id 时,如果 _id 的值不存在,刚两种方式都为插入新文档,如果 _id 的值存在,则 insert 方式无效,save 方式为更新已存在的文档
也可以同时插入多个文档,以逗号分隔,并且需要用[]括起来

db.集合名.insert([{key1:value1,key2:value2},{key3:value3,key4:value4}]);

#### 查询数据
db.集合名.find(查询条件,筛选条件);
db.集合名.find(); //查询所有数据
db.集合名.find({name:”zhangsan”}); // 相当于 select * from 集合名 where name = ‘zhangsan’;
db.集合名.find({name:”zhangsan”,age:20}); // 相当于 select * from 集合名 where name = ‘zhangsan’ and age = 20;
db.集合名.find({},{name:1,age:1}); // 相当于 select name,age from 集合名;
db.集合名.find({name:”zhangsan”},{name:1,age:1}); // 相当于 select name,age from 集合名 where name = ‘zhangsan’;
db.集合名.find({$or:[{age:22},{age:20}]}); // 相当于 select * from 集合名 where age=22 or age=20;

#### 修改数据
db.集合名.update(criteria,objNew,upsert,multi);

criteria:查询条件 // 相当于 where 条件
objNew:要设置的值 // 相当于 set 字段名=值
upsert:如果不存在是否插入新数据,true为插入,默认为false不插入
multi:true更新所有符合条件的数据,默认为false只更新第一条

db.集合名.update({name:”zhangsan”},{$set:{age:30}},false,true);
相当于 update 集合名 set age = 30 where name=’zhangsan’;

#### 删除数据
db.集合名.remove(查询条件,justOne:,writeConcern:);

justOne:为true时只删除一个文档,否则删除所有符合条件的文档
writeConcern:抛出异常的级别,默认为writeConcern.NONE

db.集合名.remove({name:”zhangsan”}); // 相当于 delete from 集合名 where name = ‘zhangsan’;
db.集合名.remove({name:”zhangsan”,true,writeConcern.NONE});

#### 条件运算符
db.集合名.find({age:30}); //查找 age=30 的数据
db.集合名.find({age:{$lt:30}}); //查找 age<30 的数据
db.集合名.find({age:{$lte:30}}); //查找 age<=30 的数据
db.集合名.find({age:{$gt:30}}); //查找 age>30 的数据
db.集合名.find({age:{$gte:30}}); //查找 age>=30 的数据
db.集合名.find({age:{$ne:30}}); //查找 age!=30 的数据

### 索引
#### 普通索引
db.集合名.ensureIndex({key:1}); // key 为要创建索引的字段,1 为指定升序创建索引,降序可以指定为 -1

#### 唯一索引
db.集合名.ensureIndex({key:1},{unique:true});

#### 复合索引
db.集合名.ensureIndex({key1:1,key2:-1});

#### 查看数据库上的所有索引
db.system.indexes.find();

#### 查看集合上的所有索引
db.集合名.getIndexes();

#### 查看 explain 执行计划
db.集合名.find({age:6}).explain();

#### 删除集合上的某个索引
db.集合名.dropIndex({key:1});

#### 删除集合上的所有索引
db.集合名.dropIndexes();

### 聚合
db.集合名.aggregate(AGGREGATE_OPERATION);

count
db.集合名.count(); // 返回集合中的文档数量
db.集合名.count({“x”:1}); // 返回 x=1 的文档的数量

distinct
db.runCommand({“distinct”:”集合名”,”key”:”键名”});

group
db.runCommand({
“group”:{
“ns”:”集合名”, “key”:”分组键”,
“initial”:{“计数器”:0},
“$reduce”:function(doc,prev){if (doc.age > prev.age) {prev.age = doc.age;}},
“condition”:{“键”:”值”}
}});
db.haha.aggregate([{$group:{_id:{name:’$name’},max:{$max:’$age’}}}]);

### 复制
主从复制
一主一从或一主多从，为了保证效率，一般从节点不超过12个,主要用于数据备份,数据恢复,读写分离
创建主节点
mkdir -p ~/dbs/master
./mongod –dbpath ~/dbs/master –port 10000 –master

创建从节点
mkdir -p ~/dbs/slave
./mongod –dbpath ~/dbs/slave –port 10001 –slave –source localhost:10000

假设主节点绑定了 localhost:27017,启动从节点时可以不添加源,而是随后向 sources 集合添加主节点信息:
./mongod –slave –dbpath ~/dbs/slave –port 27018
use local
db.sources.insert({“host”:”localhost:27017”});

### 副本集
多个节点的集群,拥有一个仲裁节点,一个主节点,多个从节点,当主节点故障时仲裁节点会把从节点变成主节点
创建三个服务器目录
mkdir ./dbs/node1
mkdir ./dbs/node2
mkdir ./dbs/arbiter

开启三个服务器放在一个副本集中
nohup mongod –replSet 副本集名称 –dbpath ./dbs/node1 –port 9927 –oplogSize 1024 &
nohup mongod –replSet 副本集名称 –dbpath ./dbs/node2 –port 9928 –oplogSize 1024 &
nohup mongod –replSet 副本集名称 –dbpath ./dbs/arbiter –port 9929 –oplogSize 1024 &

分别登录三个服务器
mongo localhost:端口号

声明配置表
config = {_id:”副本集名称”,members:[]}

向配置表中添加成员
config.members.push({_id:0,host:”localhost:9927”});
config.members.push({_id:0,host:”localhost:9928”});
config.members.push({_id:0,host:”localhost:9929”,arbiterOnly:true});

初始化配置表
rs.initiate(config);

插入数据测试
在主节点插入数据
db.haha.insert({name:”haha”,age:20});

在从节点查看数据
db.haha.find();
报错解决:rs.slaveOK();

仲裁节点不能操作数据
故障测试
把主节点 kill 掉,从节点会变成主节点
### 分片
启动配置服务器
mkdir -p ./dbs/config
mongod –dbpath ./dbs/config –port 20000

启动 mongos 进程
mongos –port 30000 –configdb localhost:20000

添加片(普通服务器或副本集)
mkdir -p ./dbs/shard1
mongod –dbpath ./dbs/shard1 –port 10000

连接 mongos 把片加入集群
mongo localhost:30000/admin
db.runCommand({addshard:”localhost:10000”,allowLocal:true});

开启数据库的分片功能
db.runCommand({“enablesharding”:”数据库名”});

根据片键对集合分片
db.runCommand({“shardcollection”:”数据库.集合名”,”key”:{“_id”:1}});