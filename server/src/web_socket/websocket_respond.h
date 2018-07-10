#ifndef __WEBSOCKET_RESPOND__
#define __WEBSOCKET_RESPOND__

#include "MySql.hpp"
#include "MySqlException.hpp"

class Websocket_Respond {
public:
	struct wsheader_type {
        unsigned header_size;
        bool fin;
        bool mask;
        enum opcode_type {
            CONTINUATION = 0x0,
            TEXT_FRAME = 0x1,
            BINARY_FRAME = 0x2,
            CLOSE = 8,
            PING = 9,
            PONG = 0xa,
        } opcode;
        int N0;
        uint64_t N;
        uint8_t masking_key[4];
    };
	Websocket_Respond(int fd);
	~Websocket_Respond();
	void send(const std::string& message);
    void sendBinary(const std::string& message);
    void sendBinary(const std::vector<uint8_t>& message);
    template<class Iterator>
    void sendData(wsheader_type::opcode_type type, uint64_t message_size, Iterator message_begin, Iterator message_end);
    string get_cmd();
    string get_ack();
    string get_name();
    string get_client_id();
    string get_time();
	int process(MySql *mysql,char *request_data ,int data_length);
	int fd_;
    string client_id;
	string client_name;
	/// mysql interface
	MySql *mysql_conn_;
	bool useMask;
	std::vector<uint8_t> rxbuf;
    std::vector<uint8_t> txbuf;
    std::vector<uint8_t> receivedData;
    std::vector<string> cmd_list;
    std::vector<string> ack_list;
    pthread_mutex_t cmd_mutex;
    pthread_mutex_t ack_mutex;
};

#endif
