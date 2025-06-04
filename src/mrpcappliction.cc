#include"mrpcapplication.h"


void mrpcapplication::Init(int argc,char **argv){

}



MrpcApplication& mrpcapplication::GetInstance(){
    static MrpcApplication app;
    return app;
}