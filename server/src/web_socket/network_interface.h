#ifndef __NETWORK_INTERFACE__
#define __NETWORK_INTERFACE__

#include "websocket_handler.h"
#include "MySql.hpp"
#include "MySqlException.hpp"
#include <json/json.h>

#define PORT 8181
#define TIMEWAIT 100
#define BUFFLEN 2048
#define MAXEVENTSSIZE 20

typedef std::map<int, Websocket_Handler *> WEB_SOCKET_HANDLER_MAP;

class Network_Interface {
public:
	Network_Interface(Json::Value config);
	~Network_Interface();
	int init(Json::Value config);
	int epoll_loop();
	int set_noblock(int fd);
	void ctl_event(int fd, bool flag);
	void run();
	void websokcet_close();
	void create_thread();
	Websocket_Handler *get_handle(string id);
	static Network_Interface *get_share_network_interface();
	//获取执行方法的指针
    void* adapter();

    static void* manage(void* pVoid);
    string adapter_status;
    pthread_t adapter_tid;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
private:
	Websocket_Handler *adapter_handler = NULL;
	//mysql interface
	MySql *mysql_conn_;
	int epollfd_;
	int listenfd_;
	WEB_SOCKET_HANDLER_MAP websocket_handler_map_;
	static Network_Interface *m_network_interface;
};

#define websocket_server Network_Interface::get_share_network_interface()

#endif
