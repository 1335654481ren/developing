#ifndef __WEBSOCKET_REQUEST__
#define __WEBSOCKET_REQUEST__

#include <stdint.h>
#include <arpa/inet.h>
#include "debug_log.h"
#include "websocket_respond.h"
#include "MySql.hpp"
#include "MySqlException.hpp"

class Websocket_Request {
public:
	Websocket_Request(int fd_);
	~Websocket_Request();
	int fetch_websocket_info(char *msg);
	void process();
	void print();
	void reset();
	/// mysql interface
	MySql *mysql_database;
private:
	int fetch_fin(char *msg, int &pos);
	int fetch_opcode(char *msg, int &pos);
	int fetch_mask(char *msg, int &pos);
	int fetch_masking_key(char *msg, int &pos);
	int fetch_payload_length(char *msg, int &pos);
	int fetch_payload(char *msg, int &pos);
private:
	uint8_t fin_;
	int fd_;
	uint8_t opcode_;
	uint8_t mask_;
	uint8_t masking_key_[4];
	uint64_t payload_length_;
	char payload_[2048];
	Websocket_Respond *respond_;
};

#endif
