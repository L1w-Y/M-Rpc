# 手写RPC框架

## 📚 目录

- [一、RPC通信原理](#一rpc通信原理)
- [二、protobuf](#二protobuf)
  - [2.1 如何使用](#21-如何使用)
  - [2.2 语法](#22-语法)
    - [Map 类型](#map-类型)
    - [Repeated 类型](#repeated-类型)
    - [枚举类型](#枚举类型)
  - [2.3 proto文件的导入](#23-proto文件的导入)
  - [2.4 package](#24-package)
  - [2.5 序列化和反序列化](#25-序列化和反序列化)
  - [2.6 在rpc通信中的序列化和反序列化](#26-在rpc通信中的序列化和反序列化)
  - [2.7 从使用角度看protobuf和rpc框架](#27-从使用角度看protobuf和rpc框架)

## 一、RPC通信原理

RPC又叫远程过程调用，它的核心原理是在分布式系统中的函数调用中，隐藏网络通信的细节，使得开发者可以像调用本地函数一样调用远程服务。

我们先从传统的单机和集群聊天服务器讲起

1. 单机服务器

![single](res/单机%20(2).png)

2. 集群服务器

为了弥补单机服务器的缺陷，自然能想到集群服务器，一个服务器不行，那我们就多弄几个

![multi](res/集群.png)

每一个服务器都有一套独立的管理系统，这种架构的优点很明显
- 并发量提升
- 每台服务器都是一个独立的服务，如果中间某台挂掉也不会影响业务
- 部署简单，一台机器与多台机器部署方式相同，如果临时需要增大并发量可以直接部署

但是缺点也比较明显：
- ，比如只修改一个小的地方，但是全部代码都需要重新编译一遍，因为模块未分开部署。
- 系统中有些模块属于CPU密集型，有些属于IO密集型，资源需求不一致，但是各个模块捆包在一起浪费资源

3. 分布式系统

![multi](res/分布式.png)

把各个模块分开，独立成单独的服务，每个模块独立部署在不同机器上，所有服务共同构成一个系统，给客户端提供服务

分布式系统灵活的点在于

- 可以根据模块的特点配备特定的硬件设备，比如CPU密集型或者IO密集型
- 对应需求量高的任务可以做成服务集群，提高并发量

在分布式系统中，比如server1调用server2的方法，内部是通过远程调用，但是通过RPC框架将这些远程的调用细节封装起来，变得像本地调用一样方便。

4. rpc通信原理

![multi](res/process%20(2).png)
图中红框内的部分就是rpc框架的作用，将通信细节封装起来

**黄色部分**：设计rpc方法参数的打包和解析，也就是数据的序列化和反序列化，使用Protobuf

**绿色部分**：网络部分，包括寻找rpc服务主机，发起rpc调用请求和响应rpc调用结果，使用muduo网络库和zookeeper服务配置中心（专门做服务发现）

<p align="right"><a href="#手写RPC框架">回到顶部⬆️</a></p>

## 二、protobuf

### 2.1 如何使用

对于我们本地文件中的数据，比如 ：
```cpp
//单一类型
int number;

//重复数据类型
int numbers[100];

// 复合数据类型
struct Person
{
    int id;
    string name;
    string sex;	
    int age;
};
```
对应的protobuf文件为：test.proto
```cpp
//版本号
syntax = "proto3";

// 定义单一类型的消息
message SingleType {
    int32 number = 1; // 单一类型的整数
}

// 定义重复数据类型的消息
message RepeatedType {
    repeated int32 numbers = 1; // 重复数据类型的整数数组
}

// 定义复合数据类型的消息
message Person {
    int32 id = 1;       // ID
    string name = 2;    // 名称
    string sex = 3;     // 性别
    int32 age = 4;      // 年龄
}
```
 
在protobuf文件中，所有的数据类型都用message包装,在使用protoc转换后，所有的message在c++中都是一个类，而message中的成员就是类的成员变量
```cpp
message 名字	
{
    // 类中的成员, 格式
    数据类型 成员名字 = 1;
    数据类型 成员名字 = 2;
    数据类型 成员名字 = 3;
	   ......     
	   ......
}
```
- message后面的名字就是生成的类的名字
- 等号后面的编号要从1开始，每个成员都有一个唯一的编号，不能重复，一般连续编号即可

.proto文件编辑好之后使用protoc工具将其转换为.pb.cc和pb.h文件
```shell
$ protoc -I path .proto文件 --cpp_out=输出路径(存储生成的c++文件)
```
在我们这个例子中，使用：
```shell
$ protoc ./Person.proto --cpp_out=.
```
文件会生成到当前目录下，这两个文件就和cpp的头文件和源文件一样，里面有我们定义以message为名字的类，同时生成set和get接口来给外界访问成员变量

### 2.2 语法

Protobuf 数据类型与 C++ 数据类型对照表

| Protobuf 数据类型 | C++ 数据类型              | 描述                                                         |
|-------------------|---------------------------|-------------------------------------------------------------|
| `double`          | `double`                 | 双精度浮点数                                                |
| `float`           | `float`                  | 单精度浮点数                                                |
| `int32`           | `int32_t` 或 `int`       | 32 位有符号整数                                             |
| `int64`           | `int64_t` 或 `long long` | 64 位有符号整数                                             |
| `uint32`          | `uint32_t` 或 `unsigned int` | 32 位无符号整数                                         |
| `uint64`          | `uint64_t` 或 `unsigned long long` | 64 位无符号整数                                     |
| `sint32`          | `int32_t` 或 `int`       | 使用 ZigZag 编码的 32 位有符号整数                          |
| `sint64`          | `int64_t` 或 `long long` | 使用 ZigZag 编码的 64 位有符号整数                          |
| `fixed32`         | `uint32_t`               | 固定大小的 32 位无符号整数                                  |
| `fixed64`         | `uint64_t`               | 固定大小的 64 位无符号整数                                  |
| `sfixed32`        | `int32_t`                | 固定大小的 32 位有符号整数                                  |
| `sfixed64`        | `int64_t`                | 固定大小的 64 位有符号整数                                  |
| `bool`            | `bool`                   | 布尔值（`true` 或 `false`）                                 |
| `string`          | `std::string`            | 字符串（UTF-8 编码）                                         |
| `bytes`           | `std::string` 或 `std::vector<uint8_t>` | 字节序列，可用于存储二进制数据                         |

#### **Map 类型**

| Protobuf 数据类型      | C++ 数据类型                       | 描述                              |
|------------------------|------------------------------------|-----------------------------------|
| `map<KeyType,ValueType>` | `std::map<KeyType, ValueType>`    | 键值对映射                       |


#### **Repeated 类型**：数组，容器等重复数据类型

**示例**：
```protobuf
repeated int32 numbers = 1;
```
对应的 C++ 类型：
```cpp
std::vector<int32_t> numbers;
```

#### **枚举类型**

**示例**：
```protobuf
enum Color {
  RED = 0;
  GREEN = 1;
  BLUE = 2;
}
```
对应的 C++ 类型：
```cpp
enum Color {
  RED = 0,
  GREEN = 1,
  BLUE = 2
};
```
<p align="right"><a href="##二、protobuf">回到章节标题⬆️</a></p>

### 2.3 proto文件的导入
在 Protocol Buffers 中，可以使用import语句在当前.ptoto中导入其它的.proto文件。这样就可以在一个.proto文件中引用并使用其它文件中定义的消息类型和枚举类型

语法如下：

```cpp
import "要使用的proto文件的名字";
```
比如刚刚提到的test.proto，我要在下面这个文件中使用其中的数据
```cpp
```protobuf
// 版本号
syntax = "proto3";

// 引入 test.proto 文件
import "test.proto";

// 使用 test.proto 中定义的消息类型
message Student {
    Person person = 1; // 使用 Person 类型
    int32 grade = 2;   // 学生成绩
}

message SingleWrapper {
    SingleType single = 1; // 使用 SingleType 类型
}

message RepeatedWrapper {
    RepeatedType repeated = 1; // 使用 RepeatedType 类型
}
```
- RepeatedType等同于repeated，前者用于自定义消息类型
- 导入的文件将会在编译时与当前文件一起被编译。
- 导入的文件也可以继续导入其他文件，形成一个文件依赖的层次结构

<p align="right"><a href="##二、protobuf">回到章节标题⬆️</a></p>

### 2.4 package
在 Protobuf 中，可以使用package关键字来定义一个消息所属的包（package）。类似于命名空间

在一个.proto文件的顶层使用package关键字来定义包：
```cpp
syntax = "proto3";
package mypackage;
message MyMessage 
{
  // ...
}
```
代表这个文件中所有的数据类型都属于mypackage命名空间下

<p align="right"><a href="##二、protobuf">回到章节标题⬆️</a></p>

### 2.5 序列化和反序列化
1. 序列化
```cpp
// 头文件目录: google\protobuf\message_lite.h
// --- 将序列化的数据 数据保存到内存中
// 将类对象中的数据序列化为字符串, c++ 风格的字符串, 参数是一个传出参数
bool SerializeToString(std::string* output) const;
// 将类对象中的数据序列化为字符串, c 风格的字符串, 参数 data 是一个传出参数
bool SerializeToArray(void* data, int size) const;

// ------ 写磁盘文件, 只需要调用这个函数, 数据自动被写入到磁盘文件中
// -- 需要提供流对象/文件描述符关联一个磁盘文件
// 将数据序列化写入到磁盘文件中, c++ 风格
// ostream 子类 ofstream -> 写文件
bool SerializeToOstream(std::ostream* output) const;
// 将数据序列化写入到磁盘文件中, c 风格
bool SerializeToFileDescriptor(int file_descriptor) const;
```
2. 反序列化
```cpp
// 头文件目录: google\protobuf\message_lite.h
bool ParseFromString(const std::string& data) ;
bool ParseFromArray(const void* data, int size);
// istream -> 子类 ifstream -> 读操作
// w->写 o: ofstream , r->读 i: ifstream
bool ParseFromIstream(std::istream* input);
bool ParseFromFileDescriptor(int file_descriptor);
```
<p align="right"><a href="##二、protobuf">回到章节标题⬆️</a></p>

### 2.6 在rpc通信中的序列化和反序列化
首先要明确的是，protobuf本身是没有任何rpc通信功能的，他只是对rpc方法的描述，通过它的描述，我们来做有关rpc请求的序列化和反序列化

例子：
```cpp
syntax = "proto3";

package faxbug; // 定义一个包名，这在 C++ 中会成为命名空间

option cc_generic_services = true;

message ResultCode {
  int32 errcode = 1;
  bytes errmsg = 2;
}

message LoginRequest
{
  bytes name = 1;
  bytes pwd = 2;
}

message LoginResponse
{
  ResultCode result = 1;
  bool success = 2;
}

message GetFrindListsRequest
{
  uint32 userid = 1;
}

message User{
  bytes name = 1;
  uint32 age = 2;
  enum Sex{
    MAN = 0;
    WOMAN = 1;
  }
  Sex sex = 3;
}

message GetFrindListsResponse
{
  ResultCode result = 1;
  repeated User friend_list = 2;
}

service UserServiceRpc
{
  rpc Login(LoginRequest) returns(LoginResponse);
  rpc GetFriendLists(GetFrindListsRequest) returns(GetFrindListsResponse);
}
```
> rpc Login(LoginRequest) returns(LoginResponse);

表示传入的是LoginRequest，返回的是LoginResponse
>rpc GetFriendLists(GetFrindListsRequest) returns(GetFrindListsResponse);

同理

```shell
protoc test.proto --cpp_out=./
```
在通过message关键字生成的类都继承自message，message基类提供了对私有成员变量的访问接口比如name() pwd() set_name() set_pwd()

而通过rpc会生成两个类
```cpp
class UserServiceRpc_Stub : public UserServiceRpc

class UserServiceRpc : public ::PROTOBUF_NAMESPACE_ID::Service
```
**生成UserServiceRpc类作为rpc服务提供者，继承自service，有核心的三个方法**
```cpp
  virtual void Login(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::faxbug::LoginRequest* request,
                       ::faxbug::LoginResponse* response,
                       ::google::protobuf::Closure* done);
  virtual void GetFriendLists(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::faxbug::GetFrindListsRequest* request,
                       ::faxbug::GetFrindListsResponse* response,
                       ::google::protobuf::Closure* done);
  const ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor* GetDescriptor();
```
而UserServiceRpc_Stub作为服务消费者，继承自UserServiceRpc，在它的内部有这么几个方法
```cpp
UserServiceRpc_Stub(::PROTOBUF_NAMESPACE_ID::RpcChannel* channel);
void Login(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::faxbug::LoginRequest* request,
                       ::faxbug::LoginResponse* response,
                       ::google::protobuf::Closure* done);
  void GetFriendLists(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::faxbug::GetFrindListsRequest* request,
                       ::faxbug::GetFrindListsResponse* response,
                       ::google::protobuf::Closure* done);
```
可以看到他同样有两个我们在test.proto中定义的方法，除此之外，他没有默认构造函数，而是传入一个RpcChannel类，我们再看Login和GetFriendLists的实现
```cpp
void UserServiceRpc_Stub::Login(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                              const ::faxbug::LoginRequest* request,
                              ::faxbug::LoginResponse* response,
                              ::google::protobuf::Closure* done) {
  channel_->CallMethod(descriptor()->method(0),
                       controller, request, response, done);
}
void UserServiceRpc_Stub::GetFriendLists(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                              const ::faxbug::GetFrindListsRequest* request,
                              ::faxbug::GetFrindListsResponse* response,
                              ::google::protobuf::Closure* done) {
  channel_->CallMethod(descriptor()->method(1),
                       controller, request, response, done);
}
```
都是调用channel_->CallMethod方法，我们再看这个方法：
```cpp
  virtual void CallMethod(const MethodDescriptor* method,
                          RpcController* controller, const Message* request,
                          Message* response, Closure* done) = 0;
```
是一个纯虚函数，需要子类重写,其实这个方法就是给到用户，在使用框架时的接口
![rpc](res/proto%20rpc.png)

<p align="right"><a href="#手写RPC框架">回到顶部⬆️</a></p>

### 2.7 从使用角度看protobuf和rpc框架

现在有这么一个本地服务，服务提供一个方法Login
```cpp
class UserService :public fixbug::UserServiceRpc
{
public:
    bool Login(std::string name,std::string pwd){
        std::cout<<"doing local service:Login"<<std::endl;
        std::cout<<"name:"<<name<<"pwd"<<pwd<<std::endl;
        return true;
    }
}
```
想要把他变成rpc远程方法，不光可以在进程内部调用，还可以在远端调用

1. 首先我们在调用一个远程的rpc方法时，肯定是需要给到实际的服务提供者`方法名`和`参数`，然后方法执行完成后提供者返回`函数的返回值`，而protobuf在其中的作用就是对双方传递的消息进行序列化和反序列化

所以proto文件的定义是这样的：
```cpp
syntax = "proto3";

package fixbug;
option cc_generic_services = true;

message ResultCode
{
    int32 errcode = 1;
    bytes errmsg = 2;
}
message LoginRequest
{
    bytes name = 1;
    bytes pwd = 2;
}
message LoginResponse
{
    ResultCode result = 1;
    bool success = 2;
}
service UserServiceRpc
{
    rpc Login(LoginRequest) returns(LoginResponse);
}
```
- 请求参数：LoginRequest
- 返回参数：LoginResponse
- rpc方法：service UserServiceRpc
- 错误码：ResultCode


2. 生成.pb.h 和 .pb.cc
得到上文提到的UserServiceRpc类（服务提供端）和UserServiceRpc_Stub类（服务消费端）
如果将本地的服务改为rpc的服务，那就是这样：
```cpp
    /*
    重写基类UserServiceRpc的虚函数，作为rpc服务提供方，当远端发起调用请求时，首先是来到rpc框架所提供的函数，来匹配本地需要做的业务，在执行完成后
    将结果重新序列化然后返回给远端
    1. caller ==> Login(LoginRequest) ==>muduo => callee
    2. callee ==> Login(LoginRequest) ==>
    */
    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done)
            {   
                //  应用获取相应的请求数据
                std::string name = request->name();
                std::string pwd = request->pwd();

                //执行本地业务
                bool login_result = Login(name,pwd);
                //写入响应，错误信息和返回值
                fixbug::ResultCode *code = response->mutable_result();
                code->set_errcode(0);
                code->set_errmsg("");
                response->set_success(login_result);
                //执行回调 执行响应消息的序列化和网络发送（由框架完成）
                done->Run();
            }

};
```


## 三、rpc框架基础类设计