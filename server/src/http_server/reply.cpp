#include "reply.hpp"
#include <string>
#include <json/json.h>

namespace http {
    namespace server {

	//响应行的string
	namespace status_strings {

	    const std::string ok =
		"HTTP/1.0 200 OK\r\n";
	    const std::string created =
		"HTTP/1.0 201 Created\r\n";
	    const std::string accepted =
		"HTTP/1.0 202 Accepted\r\n";
	    const std::string no_content =
		"HTTP/1.0 204 No Content\r\n";
	    const std::string multiple_choices =
		"HTTP/1.0 300 Multiple Choices\r\n";
	    const std::string moved_permanently =
		"HTTP/1.0 301 Moved Permanently\r\n";
	    const std::string moved_temporarily =
		"HTTP/1.0 302 Moved Temporarily\r\n";
	    const std::string not_modified =
		"HTTP/1.0 304 Not Modified\r\n";
	    const std::string bad_request =
		"HTTP/1.0 400 Bad Request\r\n";
	    const std::string unauthorized =
		"HTTP/1.0 401 Unauthorized\r\n";
	    const std::string forbidden =
		"HTTP/1.0 403 Forbidden\r\n";
	    const std::string not_found =
		"HTTP/1.0 404 Not Found\r\n";
	    const std::string internal_server_error =
		"HTTP/1.0 500 Internal Server Error\r\n";
	    const std::string not_implemented =
		"HTTP/1.0 501 Not Implemented\r\n";
	    const std::string bad_gateway =
		"HTTP/1.0 502 Bad Gateway\r\n";
	    const std::string service_unavailable =
		"HTTP/1.0 503 Service Unavailable\r\n";

	    boost::asio::const_buffer to_buffer(reply::status_type status)
	    {//char* ->boost::asio::buffer
		switch (status)
		{
		    case reply::ok:
			return boost::asio::buffer(ok);
		    case reply::created:
			return boost::asio::buffer(created);
		    case reply::accepted:
			return boost::asio::buffer(accepted);
		    case reply::no_content:
			return boost::asio::buffer(no_content);
		    case reply::multiple_choices:
			return boost::asio::buffer(multiple_choices);
		    case reply::moved_permanently:
			return boost::asio::buffer(moved_permanently);
		    case reply::moved_temporarily:
			return boost::asio::buffer(moved_temporarily);
		    case reply::not_modified:
			return boost::asio::buffer(not_modified);
		    case reply::bad_request:
			return boost::asio::buffer(bad_request);
		    case reply::unauthorized:
			return boost::asio::buffer(unauthorized);
		    case reply::forbidden:
			return boost::asio::buffer(forbidden);
		    case reply::not_found:
			return boost::asio::buffer(not_found);
		    case reply::internal_server_error:
			return boost::asio::buffer(internal_server_error);
		    case reply::not_implemented:
			return boost::asio::buffer(not_implemented);
		    case reply::bad_gateway:
			return boost::asio::buffer(bad_gateway);
		    case reply::service_unavailable:
			return boost::asio::buffer(service_unavailable);
		    default:
			return boost::asio::buffer(internal_server_error);
		}
	    }

	} // namespace status_strings

	namespace misc_strings {

	    const char name_value_separator[] = { ':', ' ' };
	    const char crlf[] = { '\r', '\n' };

	} // namespace misc_strings

	std::vector<boost::asio::const_buffer> reply::to_buffers()
	{
	    std::vector<boost::asio::const_buffer> buffers;
	    //创建响应行
	    buffers.push_back(status_strings::to_buffer(status));
	    for (std::size_t i = 0; i < headers.size(); ++i)
	    {
		header& h = headers[i];
		//响应key
		buffers.push_back(boost::asio::buffer(h.name));
		buffers.push_back(boost::asio::buffer(misc_strings::name_value_separator));
		//响应value
		buffers.push_back(boost::asio::buffer(h.value));
		buffers.push_back(boost::asio::buffer(misc_strings::crlf));
	    }
	    //crlf
	    buffers.push_back(boost::asio::buffer(misc_strings::crlf));
	    //响应报文
	    buffers.push_back(boost::asio::buffer(content));
	    return buffers;
	}

	namespace stock_replies {

	    const char ok[] = "";
	    const char created[] =
		"<html>"
		"<head><title>Created</title></head>"
		"<body><h1>201 Created</h1></body>"
		"</html>";
	    const char accepted[] =
		"<html>"
		"<head><title>Accepted</title></head>"
		"<body><h1>202 Accepted</h1></body>"
		"</html>";
	    const char no_content[] =
		"<html>"
		"<head><title>No Content</title></head>"
		"<body><h1>204 Content</h1></body>"
		"</html>";
	    const char multiple_choices[] =
		"<html>"
		"<head><title>Multiple Choices</title></head>"
		"<body><h1>300 Multiple Choices</h1></body>"
		"</html>";
	    const char moved_permanently[] =
		"<html>"
		"<head><title>Moved Permanently</title></head>"
		"<body><h1>301 Moved Permanently</h1></body>"
		"</html>";
	    const char moved_temporarily[] =
		"<html>"
		"<head><title>Moved Temporarily</title></head>"
		"<body><h1>302 Moved Temporarily</h1></body>"
		"</html>";
	    const char not_modified[] =
		"<html>"
		"<head><title>Not Modified</title></head>"
		"<body><h1>304 Not Modified</h1></body>"
		"</html>";
	    const char bad_request[] =
		"<html>"
		"<head><title>Bad Request</title></head>"
		"<body><h1>400 Bad Request</h1></body>"
		"</html>";
	    const char unauthorized[] =
		"<html>"
		"<head><title>Unauthorized</title></head>"
		"<body><h1>401 Unauthorized</h1></body>"
		"</html>";
	    const char forbidden[] =
		"<html>"
		"<head><title>Forbidden</title></head>"
		"<body><h1>403 Forbidden</h1></body>"
		"</html>";
	    const char not_found[] =
		"<html>"
		"<head><title>Not Found</title></head>"
		"<body><h1>404 Not Found</h1></body>"
		"</html>";
	    const char internal_server_error[] =
		"<html>"
		"<head><title>Internal Server Error</title></head>"
		"<body><h1>500 Internal Server Error</h1></body>"
		"</html>";
	    const char not_implemented[] =
		"<html>"
		"<head><title>Not Implemented</title></head>"
		"<body><h1>501 Not Implemented</h1></body>"
		"</html>";
	    const char bad_gateway[] =
		"<html>"
		"<head><title>Bad Gateway</title></head>"
		"<body><h1>502 Bad Gateway</h1></body>"
		"</html>";
	    const char service_unavailable[] =
		"<html>"
		"<head><title>Service Unavailable</title></head>"
		"<body><h1>503 Service Unavailable</h1></body>"
		"</html>";

	    std::string to_string(reply::status_type status)
	    {
		switch (status)
		{
		    case reply::ok:
			return ok;
		    case reply::created:
			return created;
		    case reply::accepted:
			return accepted;
		    case reply::no_content:
			return no_content;
		    case reply::multiple_choices:
			return multiple_choices;
		    case reply::moved_permanently:
			return moved_permanently;
		    case reply::moved_temporarily:
			return moved_temporarily;
		    case reply::not_modified:
			return not_modified;
		    case reply::bad_request:
			return bad_request;
		    case reply::unauthorized:
			return unauthorized;
		    case reply::forbidden:
			return forbidden;
		    case reply::not_found:
			return not_found;
		    case reply::internal_server_error:
			return internal_server_error;
		    case reply::not_implemented:
			return not_implemented;
		    case reply::bad_gateway:
			return bad_gateway;
		    case reply::service_unavailable:
			return service_unavailable;
		    default:
			return internal_server_error;
		}
	    }

	} // namespace stock_replies

	reply reply::stock_reply(reply::status_type status)
	{
	    reply rep;
	    rep.status = status;
	    //rep.content = "{\"total\":100,\"data\":[{\"id\":10001,\"name\":\"scott\"},{\"id\":10002,\"name\":\"tiger\"}]}";//stock_replies::to_string(status);
	    rep.content = stock_replies::to_string(status);
	    rep.headers.resize(2);
	    rep.headers[0].name = "Content-Length";
	    rep.headers[0].value = std::to_string(rep.content.size());
	    rep.headers[1].name = "Content-Type";
	    rep.headers[1].value = "text/html";
	    return rep;
	}
	//解析 get post的参数
	reply reply::get_post(Req_Parser &parsers, MySql *ptr_mysql)
	{
	    reply rep;
	    rep.status = reply::ok;
	    rep.content = "error";
	 //    printf("body =%s\n",parsers.body.c_str());
		// int len = parsers.numbers.size();
		// for( int i = 0; i < len; i++)
		// {
		// 	printf("%d : %s == %s\n",i, parsers.numbers[i].key.c_str(),parsers.numbers[i].value.c_str());
		// }	    
		//longin function
		if(parsers.body == "/asp/login.asp"){
			vector<USER_Table> reqq;
			char cmd_str[200];
		    ptr_mysql->query_user_info("user", "user_info", "name",parsers.numbers[0].value,cmd_str);
		    ptr_mysql->runQuery(&reqq, (const char *)cmd_str);
		    if( reqq.size() == 1){
		    	USER_Table v =	reqq.at(0);
		    	if( parsers.numbers[1].value == get<3>(v) )
		    		rep.content = "success";
		    	else
		    		rep.content = "passwd_error";
		    }else{
		    	rep.content = "not_register";
		    }
		}
		//longin function
		if(parsers.body == "/asp/register.asp"){

			USER_Table reqq(0,parsers.numbers[1].value,parsers.numbers[2].value, \
							parsers.numbers[3].value,parsers.numbers[4].value, \
							parsers.numbers[5].value,parsers.numbers[6].value, \
							atoi(parsers.numbers[7].value.c_str()));
			
		    my_ulonglong affectedRows = ptr_mysql->inster_user_info("user", "user_info", reqq);
	
		   if( affectedRows == 1){
		   		rep.content = "success";
		   }else{
		    	rep.content = "register error";
		   }
		}
		//request user_info function
		if(parsers.body == "/asp/user_info.asp"){
			Json::Value root;
	    	Json::Value arrayObj;
	    	Json::Value item;
	    	root["status"] = "success";
			vector<USER_Table> reqq;
			char cmd_str[200];
		    ptr_mysql->query_tabel_all("user", "user_info",cmd_str);
		    ptr_mysql->runQuery(&reqq, (const char *)cmd_str);
		    int len = reqq.size();
		    root["count"] = len;
		    for(int i = 0; i < len; i++){
		    	USER_Table v =	reqq.at(i);
		    	item["id"] = get<0>(v);
		    	item["name"] = get<1>(v);
		    	item["email"] = get<2>(v);
		    	item["password"] = get<3>(v);
		    	item["id_card"] = get<4>(v);
		    	item["status"] = get<5>(v);
		    	item["type"] = get<6>(v);
		    	item["age"] = get<7>(v);
		    	arrayObj.append(item);
		    }
		    root["array"] = arrayObj;
		    root.toStyledString();
		    std::string out = root.toStyledString();
	    	//std::cout << out << std::endl;
		    rep.content = out;
		}
		if(parsers.body == "/asp/station.asp"){
			Json::Value config;
			Json::Reader reader;
			Json::Value root;
			int len = parsers.numbers.size();
			if( len == 1){
				if(parsers.numbers[0].key == "station")
				{
					if (reader.parse(parsers.numbers[0].value, config))
		    		{
		    			printf("recv  the station = %s\n", config["value"].asString().c_str());
	    				//std::cout << "update status------" << std::endl;
		                my_ulonglong Rowcount = ptr_mysql->update_user_info("user", "car", (char*)"car_id", (char*)config["car_id"].asString().c_str(),(char*)config["key"].asString().c_str(),(char*)config["value"].asString().c_str());
		                if(Rowcount == 1){
		                	root["status"] = "success";
		                    std::cout << "updata station ok" << std::endl;
		                }else{
		                	root["status"] = "error";
		                    std::cout << "update station error ! !" << std::endl;
		                }
		    		}
				}
			}
			std::string out = root.toStyledString();
	    	//std::cout << out << std::endl;
		    rep.content = out;
		}
		//request user_info function
		if(parsers.body == "/asp/car_info.asp"){
			Json::Value root;
	    	Json::Value arrayObj;
	    	Json::Value item;
	    	root["status"] = "success";
			vector<CAR_Table> reqq;
			char cmd_str[200];
		    ptr_mysql->query_tabel_all("user", "car",cmd_str);
		    ptr_mysql->runQuery(&reqq, (const char *)cmd_str);
		    int len = reqq.size();
		    root["count"] = len;
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
		    root["array"] = arrayObj;
		    std::string out = root.toStyledString();
	    	//std::cout << out << std::endl;
		    rep.content = out;
		}
		//request user_info function
		if(parsers.body == "/asp/car_pos.asp"){
			//printf("/asp/car_pos.asp--------------\n");
			Json::Value config;
			Json::Reader reader;
			Json::Value root;
			root["status"] = "success";
			int len = parsers.numbers.size();
			if( len == 2){
				if(parsers.numbers[0].key == "pos")
				{
					if (reader.parse(parsers.numbers[0].value, config))
		    		{
						std::cout << "-----------sync" << std::endl;
			            if(config["type"].asString() == "pos"){ //latitude,longitude,
			                Json::Value pos = config["pos"];
			                my_ulonglong Rowcount = ptr_mysql->update_pos("user", "car", (char*)config["car_id"].asString().c_str(), config["latitude"].asDouble(), config["longitude"].asDouble(),pos["x"].asDouble(),pos["y"].asDouble(),pos["z"].asDouble(), (float)config["speed"].asDouble(), config["yaw"].asDouble(), config[
			                    "pitch"].asDouble(), config["roll"].asDouble());
			                if(Rowcount == 1){
			                    //std::cout << "update pos ok" << std::endl;
			                }else if(Rowcount == 0){
			                    std::cout << "the car_id =" << config["car_id"].asString() << "not registe on server !" <<std::endl;
			                }
			                root["status"] = "success";
			            }else if(config["type"].asString() == "status"){
			                //std::cout << "update status------" << std::endl;
			                my_ulonglong Rowcount = ptr_mysql->update_user_info("user", "car", (char*)"car_id", (char*)config["car_id"].asString().c_str(),(char*)config["key"].asString().c_str(),(char*)config["value"].asString().c_str());
			                if(Rowcount == 1){
			                    //std::cout << "updata status ok" << std::endl;
			                }else{
			                    std::cout << "update status error ! !" << std::endl;
			                }
			                root["status"] = "success";
			            }else if(config["type"].asString() == "get_station"){
			                std::cout << "update status------get_station" << std::endl;
			                vector<CAR_Table> reqq;
							char cmd_str[200];
						    ptr_mysql->query_tabel_all("user", "car",cmd_str);
						    ptr_mysql->runQuery(&reqq, (const char *)cmd_str);
						    int len = reqq.size();
						    for(int i = 0; i < len; i++){
				                CAR_Table v =    reqq.at(i);
				                root["station"] = get<5>(v);
						    }
						    root["status"] = "success";
			            }else if(config["type"].asString() == "reset_station"){
			                std::cout << "update status------" << std::endl;
			                printf("reset the station to zero!\n");
			                root["status"] = "success";
		    				//std::cout << "update status------" << std::endl;
			                my_ulonglong Rowcount = ptr_mysql->update_user_info("user", "car", (char*)"car_id", (char*)config["car_id"].asString().c_str(),(char*)config["key"].asString().c_str(),(char*)config["value"].asString().c_str());
			                if(Rowcount == 1){
			                	root["status"] = "reset_ok";
			                    std::cout << "updata station ok" << std::endl;
			                }else{
			                	root["status"] = "error";
			                    std::cout << "update station error ! !" << std::endl;
			                }
			            }
		        	}
				}
			}
		    std::string out = root.toStyledString();
	    	//std::cout << out << std::endl;
		    rep.content = out;
		}
	    rep.headers.resize(2);
	    rep.headers[0].name = "Content-Length";
	    rep.headers[0].value = std::to_string(rep.content.size());
	    rep.headers[1].name = "Content-Type";
	    rep.headers[1].value = "text/html";
	    return rep;
	}
    } // namespace server
} // namespace http
