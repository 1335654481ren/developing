#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <map>
#include <signal.h>
#include "debug_log.h"
#include "network_interface.h"
#include <json/json.h>
#include <pthread.h>

Network_Interface *Network_Interface::m_network_interface = NULL;

Network_Interface::Network_Interface(Json::Value config):
		epollfd_(0),
		listenfd_(0),
		websocket_handler_map_()
{
	if(0 != init(config)){
		exit(1);
	}else{
		// create mysql connect
		if(config["mysql"]["start"].asBool())
		{
			cout<<"websocket_server mysql start ok !\n";
			string ip = config["mysql"]["ip"].asString();
			string username = config["mysql"]["username"].asString();
			string passwd = config["mysql"]["passwd"].asString();
			mysql_conn_	 = new MySql(ip.c_str(), username.c_str(), passwd.c_str(), nullptr);
		}else{
			cout<<"websocket_server mysql is not config in config files!\n";
		}
	}
    pthread_mutex_init(&mutex,NULL);	
}

Network_Interface::~Network_Interface(){

}

Websocket_Handler *Network_Interface::get_handle(string id){
	std::map<int,Websocket_Handler *>::iterator it;
	pthread_mutex_lock(&mutex);
	if( !websocket_handler_map_.empty()){
		for(it=websocket_handler_map_.begin();it!=websocket_handler_map_.end();++it)
		{
			if( (*it->second).get_client_id() == id ){
				pthread_mutex_unlock(&mutex);
				return &(*it->second);
			}
		}
	}
	pthread_mutex_unlock(&mutex);
	return NULL;
}

void* Network_Interface::adapter(){
	std::map<int,Websocket_Handler *>::iterator it;
	printf("adapter running\n");
	while(1){
		if(!websocket_handler_map_.empty()){
			pthread_mutex_lock(&mutex);
			for(it=websocket_handler_map_.begin();it!=websocket_handler_map_.end();++it)
			{
				if( (*it->second).get_name() == "adapter" ){
					adapter_handler = &(*it->second);
					adapter_status = "login_on";
					printf("get adapter handle ok\n");
					break;
				}else{
					//std::cout << " client list= :"<< (*it->second).get_name() << std::endl;
				}
			}
			pthread_mutex_unlock(&mutex);
			usleep(500*1000);
		}else{
			printf("adapter not login on\n");
			usleep(500*1000);
			continue;
		}
		while(adapter_status == "login_on"){

			string cmd = adapter_handler->get_cmd();
			if(cmd != "empty"){
				Json::Reader reader;
			    Json::Value json_data;
			    //printf("recve length =%d,%s\n",data_length,request_data);
			    if (reader.parse(cmd, json_data))
			    {
			        //读取根节点信息  
			        string car_id = json_data["car_id"].asString();
	 				Websocket_Handler *client_handler = get_handle(car_id);
	 				if(client_handler != NULL){
 						client_handler->send(cmd);
 						client_handler = NULL;
 						cout<< "send cmd to :" << car_id << endl;
	 				}else{
	 					Json::Value root;
				        root["action"] = "ack";
				        root["name"] = "adapter";
				        root["cmdto"] = "EQ000001";
				        root["status"] = "action error, the car is not online";
				        std::string out = root.toStyledString();
	 					adapter_handler->send(out);
	 				}
			    }
			}
			usleep(100);
		}
	}
	printf("adapter exit\n");
}

void* Network_Interface::manage(void* pVoid){
    Network_Interface* p = (Network_Interface*) pVoid;
    printf("start adpater manager\n");
    p->adapter();
    return p;
}

void Network_Interface::create_thread(){
	pthread_create(&adapter_tid, NULL, manage, this);
}

int Network_Interface::init(Json::Value config){
	listenfd_ = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd_ == -1){
		DEBUG_LOG("创建套接字失败!");
		return -1;
	}
	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(sockaddr_in));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(config["websokcet"]["port"].asInt());
	if(-1 == bind(listenfd_, (struct sockaddr *)(&server_addr), sizeof(server_addr))){
		DEBUG_LOG("绑定套接字失败!");
		return -1;
	}
	if(-1 == listen(listenfd_, 5)){
		DEBUG_LOG("监听失败!");
		return -1;
	}
	adapter_status = "login_out";
	
	create_thread();

	epollfd_ = epoll_create(MAXEVENTSSIZE);

	ctl_event(listenfd_, true);	
	DEBUG_LOG("服务器启动成功!");
	return 0;
}

int Network_Interface::epoll_loop(){
	struct sockaddr_in client_addr;
	socklen_t clilen;
	int nfds = 0;
	int fd = 0;
	int bufflen = 0;
	struct epoll_event events[MAXEVENTSSIZE];
	while(true){
		nfds = epoll_wait(epollfd_, events, MAXEVENTSSIZE, TIMEWAIT);
		for(int i = 0; i < nfds; i++){
			if(events[i].data.fd == listenfd_){
				fd = accept(listenfd_, (struct sockaddr *)&client_addr, &clilen);
				ctl_event(fd, true);
			}
			else if(events[i].events & EPOLLIN){
				if((fd = events[i].data.fd) < 0)
					continue;
				Websocket_Handler *handler = websocket_handler_map_[fd];
				if(handler == NULL)
					continue;
				if((bufflen = read(fd, handler->getbuff(), BUFFLEN)) <= 0){
					ctl_event(fd, false);
				}
				else{
					handler->process();
					if(adapter_status == "login_on"){
						string ack = handler->get_ack();
						if(ack != "empty" && adapter_handler != NULL){
							adapter_handler->send(ack);
						}
					}
				}
			}
		}
	}
	return 0;
}

int Network_Interface::set_noblock(int fd){
	int flags;
    if ((flags = fcntl(fd, F_GETFL, 0)) == -1)
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void Network_Interface::websokcet_close(){
	close(listenfd_);
}

Network_Interface *Network_Interface::get_share_network_interface(){
	if(m_network_interface == NULL)
		;//m_network_interface = new Network_Interface();
	return m_network_interface;
}

void Network_Interface::ctl_event(int fd, bool flag){
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = flag ? EPOLLIN : 0;
	epoll_ctl(epollfd_, flag ? EPOLL_CTL_ADD : EPOLL_CTL_DEL, fd, &ev);
	if(flag){
		set_noblock(fd);
		pthread_mutex_lock(&mutex);
		websocket_handler_map_[fd] = new Websocket_Handler(fd);
		pthread_mutex_unlock(&mutex);
		if(fd != listenfd_){
			adapter_status = "login_new";
			websocket_handler_map_[fd]->database = mysql_conn_;
			websocket_handler_map_[fd]->set_mysql_inferface();
			DEBUG_LOG("fd: %d 加入epoll循环", fd);
		}
	}
	else{
		close(fd);
		if(websocket_handler_map_[fd]->get_name() == "adapter"){
			adapter_handler = NULL;
			adapter_status = "login_out";
			printf("adapter login out!\n");
		}else if(websocket_handler_map_[fd]->get_name() == "car"){
			if(adapter_handler != NULL){
				Json::Value root;
		        root["action"] = "report";
		        root["name"] = "management";
		        root["id"] = websocket_handler_map_[fd]->get_client_id();
		        root["status"] = "car login_out";
		        std::string out = root.toStyledString();
				adapter_handler->send(out);
				printf("car login out!\n");
			}
		    std::cout << "update status------" << std::endl;
		    string client_id = websocket_handler_map_[fd]->get_client_id();
            my_ulonglong Rowcount = mysql_conn_->update_user_info("user", "car", (char*)"car_id", (char*)client_id.c_str(), (char*)"car_status",(char*)"offline");
            if(Rowcount == 1){
                std::cout << "updata status ok" << std::endl;
            }else{
                std::cout << "update status error ! !" << std::endl;
            }
		}
		pthread_mutex_lock(&mutex);
		std::cout << "name + id = " << websocket_handler_map_[fd]->get_name() << "\t\t"<< websocket_handler_map_[fd]->get_client_id() << std::endl;
		delete websocket_handler_map_[fd];
		websocket_handler_map_.erase(fd);
		pthread_mutex_unlock(&mutex);
		DEBUG_LOG("fd: %d 退出epoll循环", fd);
	}
}

void Network_Interface::run(){
	epoll_loop();
}
