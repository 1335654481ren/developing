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
    if(data_length < 100){
        printf("error  recve length =%d,%s\n",data_length,request_data);
        return 0;
    }
    if (reader.parse(request_data, config))
    {
        //std::cout << "json type ok" << std::endl;
        //读取根节点信息  
        string action = config["action"].asString();
        if(action == "register"){
            std::cout << "action :register" << std::endl;
            client_name = config["name"].asString();
            if(client_name == "car"){
                client_id = config["car_id"].asString();
                std::cout << "client id :" << client_id << std::endl;
                vector<CAR_Table> reqq;
                char cmd_str[200];
                mysql->query_user_info("user", "car", "car_id",client_id,cmd_str);
                mysql->runQuery(&reqq, (const char *)cmd_str);
                if( reqq.size() == 1){
                    CAR_Table v =  reqq.at(0);
                    if( client_id == get<1>(v) ){
                        std::cout << "car: " << client_id << "login"<< std::endl;
                    }
                }else{
                    Json::Value pos = config["pos"];
                    std::cout << "---pos" << std::endl;
                    CAR_Table reqq(0,config["car_id"].asString(),config["user_id"].asString(), 
                    config["status"].asString(),config["car_status"].asString(), config["sensor_status"].asString(),
                    config["latitude"].asDouble(),config["longitude"].asDouble(),pos["x"].asDouble(),pos["y"].asDouble(),pos["z"].asDouble(),(float)config["speed"].asDouble(), 
                    config["yaw"].asDouble(),config["pitch"].asDouble(),config["roll"].asDouble());
                    my_ulonglong affectedRows = mysql->inster_car_info("user", "car", reqq);

                    if( affectedRows == 1){
                        std::cout << " register success" << std::endl;
                    }
                }

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
                client_id = config["user_id"].asString();
                root["status"] = "success";
                root["description"] = "adapter register ok";
                printf("adapter login on\n");
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
            if(cmd_list.size() < 100){
                printf("add a cmd\n");
                cmd_list.push_back(request_data);
            }else{
                cmd_list.erase(cmd_list.begin());
            }
            pthread_mutex_unlock(&cmd_mutex);
        }else if(action == "sync"){
            //std::cout << "-----------sync" << std::endl;
            if(config["type"].asString() == "pos"){ //latitude,longitude,
                Json::Value pos = config["pos"];
                my_ulonglong Rowcount = mysql->update_pos("user", "car", (char*)config["car_id"].asString().c_str(), config["latitude"].asDouble(), config["longitude"].asDouble(),pos["x"].asDouble(),pos["y"].asDouble(),pos["z"].asDouble(), (float)config["speed"].asDouble(), config["yaw"].asDouble(), config[
                    "pitch"].asDouble(), config["roll"].asDouble());
                if(Rowcount == 1){
                    //std::cout << "update pos ok" << std::endl;
                }else if(Rowcount == 0){
                    std::cout << "the car_id =" << config["car_id"].asString() << "not registe on server !" <<std::endl;
                }
            }else if(config["type"].asString() == "status"){
                //std::cout << "update status------" << std::endl;
                my_ulonglong Rowcount = mysql->update_user_info("user", "car", (char*)"car_id", (char*)config["car_id"].asString().c_str(),(char*)config["key"].asString().c_str(),(char*)config["value"].asString().c_str());
                if(Rowcount == 1){
                    //std::cout << "updata status ok" << std::endl;
                }else{
                    std::cout << "update status error ! !" << std::endl;
                }
            }
        }else if(action == "use_sync"){
            if(config["type"] == "init"){
                vector<CAR_Table> reqq;
                char cmd_str[200];
                mysql->query_tabel_all("user", "car",cmd_str);
                mysql->runQuery(&reqq, (const char *)cmd_str);
                int len = reqq.size();
                Json::Value ack,item,arrayObj;
                ack["action"] = "ack";
                ack["status"] = "error";
                ack["name"] = "management";
                ack["time"] = get_time();
                ack["count"] = len;
                for(int i = 0; i < len; i++){
                    CAR_Table v =    reqq.at(i);
                    item["id"] = get<0>(v);
                    item["car_id"] = get<1>(v);
                    item["user_id"] = get<2>(v);
                    item["status"] = get<3>(v);
                    item["car_status"] = get<4>(v);
                    item["sensor_status"] = get<5>(v);
                    item["latitude"] = get<6>(v);
                    item["longitude"] = get<7>(v);
                    item["x"] = get<8>(v);
                    item["y"] = get<9>(v);
                    item["z"] = get<10>(v);
                    item["speed"] = get<11>(v);
                    item["yaw"] = get<12>(v);
                    item["pitch"] = get<13>(v);
                    item["roll"] = get<14>(v);
                    arrayObj.append(item);
                }
                ack["array"] = arrayObj;
                std::string out = ack.toStyledString();
                send(out);
                //std::cout << out << std::endl;
            }else if(config["type"] == "connect_car"){

            }else if(config["type"] == "req_pos"){

            }else if(config["type"] == "cmd"){

            }
        }  
        //int age = config["age"].asInt();  
        // bool sex_is_male = config["sex_is_male"].asBool();  
    }
    std::string send_msg = root.toStyledString();
    //send(send_msg);
    //printf("send string to:%s\n", send_msg.c_str());
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

