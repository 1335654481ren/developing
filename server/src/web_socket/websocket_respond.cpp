#include <stdio.h>
#include <unistd.h>
#include "websocket_respond.h"

Websocket_Respond::Websocket_Respond(int fd):fd_(fd)
{
	printf("fd === %d\n",fd_);
}

Websocket_Respond::~Websocket_Respond() {

}

int Websocket_Respond::process(MySql *mysql, char *request_data ,int data_length)
{
	char buff[1024];
	int len = 0;
	buff[len++] = 0x81;
	buff[len++] = 5;
	buff[len++] = 'r';
	buff[len++] = 'e';
	buff[len++] = 'n';
	buff[len++] = 'x';
	buff[len++] = 'l';
	write(fd_, buff, len);
	write(fd_, buff, len);
	write(fd_, buff, len);
	printf("recve length =%d,%s\n",data_length,request_data);
	typedef tuple<
	    unique_ptr<int>,
	    unique_ptr<string>,
	    unique_ptr<string>,
	    unique_ptr<string>,
	    unique_ptr<string>,
	    unique_ptr<string>,
	    unique_ptr<string>,
	    unique_ptr<int>
	> autoPtrUserTuple;
	vector<autoPtrUserTuple> autoPtrUsers;
	mysql->runCommand("USE user");
	mysql->runQuery(&autoPtrUsers, "SELECT * FROM user_info");
	printTupleVector(autoPtrUsers);
	return 0;
}

