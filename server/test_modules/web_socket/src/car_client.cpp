#include "easywsclient.hpp"
//#include "easywsclient.cpp" // <-- include only if you don't want compile separately
#ifdef _WIN32
#pragma comment( lib, "ws2_32" )
#include <WinSock2.h>
#endif
#include <assert.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <json/json.h>
;
using easywsclient::WebSocket;
static WebSocket::pointer ws = NULL;

int flag = 0;

void handle_message(const std::string & message)
{
    if(flag == 0){
        Json::Reader reader;
        Json::Value config;
        //printf("recve length =%d,%s\n",data_length,request_data);
        if (reader.parse(message, config))
        {
            //读取根节点信息  
            std::string action = config["action"].asString();
            if( action == "cmd"){
                Json::Value root;
                root["action"] = "ack";
                root["name"] = "car";
                root["id"] = "EQ000001";
                root["status"] = "cmd action sucessesful";
                std::string out = root.toStyledString();
                ws->send(out);
            }
        }        
    }
    printf("get message>>> %s\n", message.c_str());
}

int main(int argc, char **argv)
{
    if(argc >1)
    {flag = 1; printf("run as adapter!\n");}else{printf("run as a car client!\n");}
#ifdef _WIN32
    INT rc;
    WSADATA wsaData;

    rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (rc) {
        printf("WSAStartup Failed.\n");
        return 1;
    }
#endif

    ws = WebSocket::from_url("ws://localhost:8181");
    assert(ws);

    pid_t pid = fork();
    if(pid < 0 ){
        printf("run error!\n");
    }else if(pid == 0){
        if(flag){
            Json::Value root;
            root["action"] = "register";
            root["name"] = "adapter";
            std::string out = root.toStyledString();
            ws->send(out);
            while(1){
                std::string cmd;
                //printf("\033[2J");
                //std::cout <<"input cmd:"; 
                //std::cin >> cmd;
                Json::Value msg;
                msg["action"] = "cmd";
                msg["id"] = "EQ000001";
                msg["cmd"] = "start action";
                std::string out = msg.toStyledString();
                ws->send(out);
                usleep(1000*100);       
            }            
        }else{
            Json::Value root;
            root["action"] = "register";
            root["name"] = "car";
            root["id"] = "EQ000001";
            std::string out = root.toStyledString();
            ws->send(out);
            int count = 0;
            while(1){
                std::string cmd;
                //printf("\033[2J");
                std::cout <<"input cmd:"; 
                //std::cin >> cmd;
                std::cout<<"count = "<<count<<std::endl;
                Json::Value root;
                root["action"] = "ack";
                root["name"] = "car";
                root["id"] = "EQ000001";
                root["report"] = count++;
                std::string out = root.toStyledString();
                ws->send(out);
                usleep(10000);       
            }
        }

    }else{
        while (ws->getReadyState() != WebSocket::CLOSED) {
          ws->poll();
          ws->dispatch(handle_message);
        }
    }
    delete ws;
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}