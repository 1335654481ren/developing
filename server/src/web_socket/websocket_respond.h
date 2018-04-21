#ifndef __WEBSOCKET_RESPOND__
#define __WEBSOCKET_RESPOND__

#include "MySql.hpp"
#include "MySqlException.hpp"

class Websocket_Respond {
public:
	Websocket_Respond(int fd);
	~Websocket_Respond();
	int process(MySql *mysql,char *request_data ,int data_length);
	int fd_;
	/// mysql interface
	MySql *mysql_conn_;
};

#endif
