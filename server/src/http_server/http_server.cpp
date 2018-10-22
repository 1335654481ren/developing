#include "http_server.hpp"
#include <signal.h>
#include <utility>
#include <json/json.h>

namespace http {
    namespace server {

	server::server(Json::Value config)
	    : io_service_(),
	    signals_(io_service_),
	    acceptor_(io_service_),
	    connection_manager_(),
	    socket_(io_service_),
	    request_handler_(config["server"]["root"].asString())//处理请求
	{
	    // Register to handle the signals that indicate when the server should exit.
	    // It is safe to register for the same signal multiple times in a program,
	    // provided all registration for the specified signal is made through Asio.
	    signals_.add(SIGINT);
	    signals_.add(SIGTERM);
#if defined(SIGQUIT)
	    signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)

	    do_await_stop();
		// create mysql connect
		if(config["mysql"]["start"].asBool())
		{
			cout<<"http_server mysql start ok !\n";
			string ip = config["mysql"]["ip"].asString();
			string username = config["mysql"]["username"].asString();
			string passwd = config["mysql"]["passwd"].asString();
			mysql_conn_	 = new MySql(ip.c_str(), username.c_str(), passwd.c_str(), nullptr);
			// mysql_conn_->runCommand("DROP DATABASE IF EXISTS user");
		 //    mysql_conn_->runCommand("CREATE DATABASE user");
		 //    mysql_conn_->runCommand("USE user");
		 //    mysql_conn_->runCommand("DROP TABLE IF EXISTS user_info");
		 //    mysql_conn_->runCommand(
		 //        "CREATE TABLE user_info ("
		 //            "id INT NOT NULL AUTO_INCREMENT,"
		 //            "PRIMARY KEY(id),"
		 //            "name VARCHAR(64) NOT NULL,"
		 //            "email CHAR(64) NOT NULL,"
		 //            "password CHAR(64) NOT NULL,"
		 //            "id_card CHAR(64) NOT NULL,"
		 //            "status CHAR(64) NOT NULL,"
		 //            "type CHAR(64) NOT NULL,"
		 //            "age INT)");
		    // mysql_conn_->runCommand("USE user");
		    // mysql_conn_->runCommand("DROP TABLE IF EXISTS car");
		    // mysql_conn_->runCommand(
		    //     "CREATE TABLE car ("
		    //         "id INT NOT NULL AUTO_INCREMENT,"
		    //         "PRIMARY KEY(id),"
		    //         "car_id VARCHAR(64) NOT NULL,"
		    //         "user_id CHAR(64) NOT NULL,"
		    //         "status CHAR(64) NOT NULL,"
		    //         "car_status CHAR(64) NOT NULL,"
		    //         "sensor_status CHAR(64) NOT NULL,"
		    //         "latitude DOUBLE,"
		    //         "longitude DOUBLE,"
		    //         "x DOUBLE,"
		    //         "y DOUBLE,"
		    //         "z DOUBLE,"
		    //         "speed FLOAT,"
		    //         "yaw DOUBLE,"
		    //         "pitch DOUBLE,"
		    //         "roll DOUBLE)");

			request_handler_.set_mysql(mysql_conn_);
		}else{
			cout<<"http_server mysql is not config in config files!\n";
		}
		
	    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
	    boost::asio::ip::tcp::resolver resolver(io_service_);
	    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve({config["server"]["ip"].asString(), std::to_string(config["server"]["port"].asInt())});
	    acceptor_.open(endpoint.protocol());
	    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	    acceptor_.bind(endpoint);
	    acceptor_.listen();

	    do_accept();
	}

	void server::run()
	{
	    // The io_service::run() call will block until all asynchronous operations
	    // have finished. While the server is running, there is always at least one
	    // asynchronous operation outstanding: the asynchronous accept call waiting
	    // for new incoming connections.
	    io_service_.run();
	}

	void server::do_accept()//异步接收连接
	{
	    acceptor_.async_accept(socket_,[this](boost::system::error_code ec)
		    {
			    // Check whether the server was stopped by a signal before this
			    // completion handler had a chance to run.
			    if (!acceptor_.is_open())
			    {
			    	return;
			    }

			    if (!ec)
			    {
			    	connection_manager_.start(std::make_shared<connection>(
				    	std::move(socket_), connection_manager_, request_handler_, mysql_conn_));
			    }
		    	do_accept();
		    });
	}

	void server::do_await_stop()
	{
	    signals_.async_wait([this](boost::system::error_code /*ec*/, int /*signo*/)
		    {
			    // The server is stopped by cancelling all outstanding asynchronous
			    // operations. Once all operations have finished the io_service::run()
			    // call will exit.
			    acceptor_.close();
			    connection_manager_.stop_all();
		    });
	}

    } // namespace server
} // namespace http
