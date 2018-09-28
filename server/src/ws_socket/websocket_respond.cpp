#include <stdio.h>
#include <unistd.h>
#include "websocket_respond.h"
#include "json/json.h"
#include "global.h"

Websocket_Respond::Websocket_Respond(int fd):fd_(fd)
{
	pthread_mutex_init(&ack_mutex,NULL);
    pthread_mutex_init(&cmd_mutex,NULL);
}

Websocket_Respond::~Websocket_Respond() {

}
string Websocket_Respond::get_cmd(){
    pthread_mutex_lock(&cmd_mutex);   
    if(!cmd_list.empty()){
        string cmd = cmd_list[0];
        cmd_list.erase(cmd_list.begin());
        pthread_mutex_unlock(&cmd_mutex); 
        return cmd;
    }
    pthread_mutex_unlock(&cmd_mutex); 
    return "empty";
}
string Websocket_Respond::get_ack(){
    pthread_mutex_lock(&ack_mutex);
    if(!ack_list.empty()){
        string ack = ack_list[0];
        ack_list.erase(ack_list.begin());
        pthread_mutex_unlock(&ack_mutex);
        return ack;
    }
    pthread_mutex_unlock(&ack_mutex);
    return "empty";
}
string Websocket_Respond::get_name(){
    return client_name;
}
string Websocket_Respond::get_client_id(){
    return client_id;
}
void Websocket_Respond::send(const std::string& message) {
    sendData(wsheader_type::TEXT_FRAME, message.size(), message.begin(), message.end());
}

void Websocket_Respond::sendBinary(const std::string& message) {
    sendData(wsheader_type::BINARY_FRAME, message.size(), message.begin(), message.end());
}

void Websocket_Respond::sendBinary(const std::vector<uint8_t>& message) {
    sendData(wsheader_type::BINARY_FRAME, message.size(), message.begin(), message.end());
}
template<class Iterator>
void Websocket_Respond::sendData(wsheader_type::opcode_type type, uint64_t message_size, Iterator message_begin, Iterator message_end) {
    // TODO:
    // Masking key should (must) be derived from a high quality random
    // number generator, to mitigate attacks on non-WebSocket friendly
    // middleware:
    const uint8_t masking_key[4] = { 0x12, 0x34, 0x56, 0x78 };
    // TODO: consider acquiring a lock on txbuf...
    std::vector<uint8_t> header;
    header.assign(2 + (message_size >= 126 ? 2 : 0) + (message_size >= 65536 ? 6 : 0) + (useMask ? 4 : 0), 0);
    header[0] = 0x80 | type;
    if (false) { }
    else if (message_size < 126) {
        header[1] = (message_size & 0xff) | (useMask ? 0x80 : 0);
        if (useMask) {
            header[2] = masking_key[0];
            header[3] = masking_key[1];
            header[4] = masking_key[2];
            header[5] = masking_key[3];
        }
    }
    else if (message_size < 65536) {
        header[1] = 126 | (useMask ? 0x80 : 0);
        header[2] = (message_size >> 8) & 0xff;
        header[3] = (message_size >> 0) & 0xff;
        if (useMask) {
            header[4] = masking_key[0];
            header[5] = masking_key[1];
            header[6] = masking_key[2];
            header[7] = masking_key[3];
        }
    }
    else { // TODO: run coverage testing here
        header[1] = 127 | (useMask ? 0x80 : 0);
        header[2] = (message_size >> 56) & 0xff;
        header[3] = (message_size >> 48) & 0xff;
        header[4] = (message_size >> 40) & 0xff;
        header[5] = (message_size >> 32) & 0xff;
        header[6] = (message_size >> 24) & 0xff;
        header[7] = (message_size >> 16) & 0xff;
        header[8] = (message_size >>  8) & 0xff;
        header[9] = (message_size >>  0) & 0xff;
        if (useMask) {
            header[10] = masking_key[0];
            header[11] = masking_key[1];
            header[12] = masking_key[2];
            header[13] = masking_key[3];
        }
    }
    // N.B. - txbuf will keep growing until it can be transmitted over the socket:
    txbuf.insert(txbuf.end(), header.begin(), header.end());
    txbuf.insert(txbuf.end(), message_begin, message_end);
    if (useMask) {
        size_t message_offset = txbuf.size() - message_size;
        for (size_t i = 0; i != message_size; ++i) {
            txbuf[message_offset + i] ^= masking_key[i&0x3];
        }
    }

    while (txbuf.size()) {
    	int ret = write(fd_, (char*)&txbuf[0], txbuf.size());
         
        if (false) { } // ??
        else if (ret <= 0) {
            fputs(ret < 0 ? "Connection error!\n" : "Connection closed!\n", stderr);
            break;
        }
        else {
            txbuf.erase(txbuf.begin(), txbuf.begin() + ret);
        }
    }
}

string Websocket_Respond::get_time()
{
    time_t timer;//time_t就是long int 类型
    struct tm *tblock;
    timer = time(NULL);
    tblock = localtime(&timer);
    string str_time = asctime(tblock);
    return str_time;
}

int Websocket_Respond::process(MySql *mysql, char *request_data ,int data_length)
{
    Json::Value root;
    root["action"] = "ack";
    root["status"] = "error";
    root["name"] = "management";
    root["time"] = get_time();
    Json::Reader reader;
    Json::Value config;
    //printf("recve length =%d,%s\n",data_length,request_data);
    if (reader.parse(request_data, config))
    {
        //读取根节点信息  
        string action = config["action"].asString();
        if(action == "register"){
            client_name = config["name"].asString();
            if(client_name == "car"){
                client_id = config["id"].asString();
                Json::Value report;
                report["action"] = "report";
                report["name"] = "management";
                report["description"] = "New car register on ID:" + client_id;
                pthread_mutex_lock(&ack_mutex);
                if(ack_list.size() < 100)
                    ack_list.push_back(report.toStyledString());
                else{
                    ack_list.erase(ack_list.begin());
                }
                pthread_mutex_unlock(&ack_mutex);
                root["status"] = "success";
                root["description"] = "car register ok";
            }else if(client_name == "adapter"){
                root["status"] = "success";
                root["description"] = "adapter register ok";
            } 
            std::cout<< "register :" << client_name << std::endl;

        }else if(action == "ack"){
            pthread_mutex_lock(&ack_mutex);
            if(ack_list.size() < 100)
                ack_list.push_back(request_data);
            else{
                ack_list.erase(ack_list.begin());
            }            
            pthread_mutex_unlock(&ack_mutex);
            root["status"] = "success";
        }else if(action == "cmd"){
            root["status"] = "success";
            pthread_mutex_lock(&cmd_mutex);
            if(cmd_list.size() < 100)
                cmd_list.push_back(request_data);
            else{
                cmd_list.erase(cmd_list.begin());
            }
            pthread_mutex_unlock(&cmd_mutex);
        }  
        //int age = config["age"].asInt();  
        // bool sex_is_male = config["sex_is_male"].asBool();  
    }
    std::string send_msg = root.toStyledString();
    send(send_msg);

	// typedef tuple<
	//     unique_ptr<int>,
	//     unique_ptr<string>,
	//     unique_ptr<string>,
	//     unique_ptr<string>,
	//     unique_ptr<string>,
	//     unique_ptr<string>,
	//     unique_ptr<string>,
	//     unique_ptr<int>
	// > autoPtrUserTuple;
	// vector<autoPtrUserTuple> autoPtrUsers;
	// mysql->runCommand("USE user");
	// mysql->runQuery(&autoPtrUsers, "SELECT * FROM user_info");
	// printTupleVector(autoPtrUsers);
	return 0;
}

