#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "http_server.hpp"
#include "network_interface.h"
#include <json/json.h>

#include <fstream>  
#include <sstream>  

Network_Interface *websocket_serverd = NULL;

void catch_signal(int sign)
{
    switch (sign)
    {
    case SIGINT:
    	if(websocket_serverd != NULL){
    		websocket_serverd->websokcet_close();
    	}
        printf("ctrl + C 被执行了!\n");
        //exit(0);
        break;
    }
}

int main(int argc, char* argv[])
{
    std::ifstream is;  
    is.open ("./config.json", std::ios::binary );
    if( !is.is_open() )    
    {   
	    cout << "Error opening file\n";   
	    return 0;   
    }     
	Json::Reader reader;
	Json::Value config;
	if (reader.parse(is, config))
	{
		//读取根节点信息  
	    string name = config["name"].asString();  
	    //int age = config["age"].asInt();  
	    // bool sex_is_male = config["sex_is_male"].asBool();  
	  
	    cout << "My name is " << name << endl;  
	    cout << "My phone number:  " << config["phone"].asString() << endl;
	    cout << "My Email:  " << config["Email"].asString() << endl;
	    //读取数组信息  
	    cout << "Here's my services:" << endl;  
	    for(unsigned int i = 0; i < config["services"].size(); i++)  
	    {  
	        string ach = config["services"][i].asString();  
	        cout << ach << '\t';  
	    }
	    cout <<endl;

 	}else{
 		cout << "Can not find the config file(config.json)!\n";
 		exit(0);
 	}

    if(signal(SIGQUIT,catch_signal)==SIG_ERR)
    {
        //注册新号失败
        perror("signal error");
    }

	pid_t pid = fork();
	if(pid < 0 ){
		printf("run error!\n");
	}else if(pid == 0){
		if(config["websokcet"]["start"].asBool())
		{
			cout<<"websocket_server start ok !\n";
			websocket_serverd = new Network_Interface(config);
			websocket_serverd->run();
			pthread_join(websocket_serverd->adapter_tid, NULL); 
		}else{
			cout<<"websocket_server is not config in config files!\n";
		}
	}else{
		//printf("this is parent\n");
	    try
	    {
	    	if(config["server"]["start"].asBool()){
	    		cout<<"http_server start ok !\n";

				http::server::server http_server(config);
				// Run the server until stopped.
				http_server.run();
	    	}else{
	    		cout<<"http_server is not config in config files!\n";
	    	}

	    }
	    catch (std::exception& e)
	    {
			std::cerr << "exception: " << e.what() << "\n";
	    }		
	}

    return 0;
}
