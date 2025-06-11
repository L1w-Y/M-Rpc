#pragma once
#include<google/protobuf/service.h>
#include<string>

class MrpcController : public google::protobuf::RpcController
{
public:
    MrpcController();
    void Reset();
    bool Failed() const;
    void SetFailed(const std::string& reason);

    void StartCancel() override;
    bool IsCanceled() const;
    std::string ErrorText()const;
    void NotifyOnCancel(google::protobuf::Closure* callback);

private:
    bool m_failed;
    std::string m_errText;
};