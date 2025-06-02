#include <iostream>
#include <fstream> // 用于文件操作 (可选)
#include <string>
#include "test.pb.h" // 由 protoc 根据 person.proto 生成的头文件
#include <google/protobuf/stubs/common.h> // 包含 GOOGLE_PROTOBUF_VERIFY_VERSION

int main() {

    example_package::Person person;

    // 设置字段值
    person.set_name("L1w-Y"); // 使用你的用户名 :)
    person.set_id(20250601);  
    person.set_email("l1w-y@example.com");

    std::cout << "原始 Person 数据:" << std::endl;
    std::cout << "  Name: " << person.name() << std::endl;
    std::cout << "  ID: " << person.id() << std::endl;
    std::cout << "  Email: " << person.email() << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    // 1. 序列化到字符串
    std::string serialized_str;
    if (!person.SerializeToString(&serialized_str)) {
        std::cerr << "错误: 序列化 Person 到字符串失败." << std::endl;
        google::protobuf::ShutdownProtobufLibrary(); // 在退出前清理
        return -1;
    }
    std::cout << "序列化后的字符串 (长度 " << serialized_str.length() << "):" << std::endl;
    // 注意：序列化后的字符串是二进制数据，直接打印可能包含不可见字符或乱码
    // 为了演示，这里简单打印，但实际应用中通常直接传输或存储二进制数据
    // for (char c : serialized_str) {
    //    std::cout << std::hex << (0xFF & static_cast<int>(c)) << " ";
    // }
    // std::cout << std::dec << std::endl;
    std::cout << "(二进制内容，可能无法直接阅读)" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    // 2. 从字符串反序列化
    example_package::Person parsed_person;
    if (!parsed_person.ParseFromString(serialized_str)) {
        std::cerr << "错误: 从字符串反序列化 Person 失败." << std::endl;
        google::protobuf::ShutdownProtobufLibrary(); // 在退出前清理
        return -1;
    }

    std::cout << "从字符串反序列化后的 Person 数据:" << std::endl;
    std::cout << "  Name: " << parsed_person.name() << std::endl;
    std::cout << "  ID: " << parsed_person.id() << std::endl;
    std::cout << "  Email: " << parsed_person.email() << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}