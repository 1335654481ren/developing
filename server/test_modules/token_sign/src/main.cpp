#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include <iostream>                                                 
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include "otacli/ota_client.h"
#include "otacli/hobot_utils.h"
#include "otacli/ota_typedef.h"

#include "client_wss.hpp" 
#include "carmsg.pb.h"

#include <cstdio>
//#include <opencv2/opencv.hpp>  

using namespace std;
//using namespace cv;

#define TEST_TAG "hobottest"

// const char *tenant_token ="eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9."
// "eyJleHAiOjE4NDE5MDM0OTAsImp0aSI6IjU2ZGQ3YWRiLWU5NjktNDhlZS04NjUyLTBmYzA1ZjBhNzAwNCIsImlzcyI6IkhvYm90Iiwic3ViIjoidGVzdDI"
// "yQGhvYm90LmNjIiwic2NwIjoiaG9ib3QuKiIsImhvYm90LnRlbmFudCI6Im1pZGVhIn0.eQp2qBaoOEVjulEtxyvn1GTWBC_g-"
// "_djxrLJRCdDgY9rgWMxOhLy9Wodme0hSXBOt9ObnKq8bGiTENumxsesrocHhtkBykbfJz7ko9c2KsK5ltBF3D6Ff4rU2WMdF46MzsIfmdw98PfZrGavMQAC"
// "9TqfybsfKTtvDaEKtJkqg_5WCf97NHed9mYcsoq_6gUVy6nc7t3NCyenJ8f8sg-"
// "cub6cOo4QOwKO2Fb4qDOyIszHwfFxSDmSKSLrgNB_Nl0VofjX6YkyV8IK8CsY26MJ93ZFTRsIIOZnqnBVbDl1J49Em-DgXaBimeq3eR25bNcKS7IQ5AV-"
// "AW8JUX91bgu7gPnpuj6JGIyJ6Nk6Sk4S-w59wPUukvDFqRc67uDgyG7RnrQFMT9ihctYz22zZ56OPMbJhByqIr_gX7DP2CDEGUbzhIgNBQ-"
// "kkRxn1GnvGzGEJVZR6EG0KOv6ougwhCFQPt0NhHEeoCti69nHtblIS5QoKMzFsvLjfCDMcnN7K0nY";

const char *tenant_token ="eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE4NTAzNzY0MTMsImp0aSI6IjM0MGZiZDg5LWExZTYtNGYxYS05ZTQzLTc5OTZlNDYyZTUwMCIsImlzcyI6IkhvYm90Iiwic3ViIjoienN5dGVzdGF1dG9AaG9ib3QuY2MiLCJzY3AiOiJob2JvdC4qIiwiaG9ib3QudGVuYW50IjoienN5dGVzdDEwIn0.ZBllpqFxKaJq_D7R0CYK5rxYgjWVIazN7_OsGZvIEKdLztZJAL5azWft5rFN1DZ_jSwd4gUrVuQgAiYDqO3qIug4tdQstFrS6EAplvqSIiJykCUj7XWWpF_UikZ4kLhsfvuHNamOF9I20PbsH7DxL5xmqRBo2BBznLCGWGRyeFyQ8l_j4Ed_wLvz4V1PXFA9ZKGmuvCYGluCrl7HVmmPBktjhEhqxaAygLd-E7vAyMMjHcILMG6kEe1-fOZHhPduSW3mWvb8HisVS9Ze5xsylhzq-yKp4TZLCeHo08v_Y_hjulCK3bMLDCZBCBwdBgti4CHXCtvy3Vr3eAzc5hTZNnO-D02_SZj_Mg5qzlrkPz87jtVFnKA4u2w0IGyYXcT585vySRPxhyfBjUe-y8vLEwcaANGrvrxHVpjfqeexL3TWxpV31jyVDxdSklPXuEKtAPSt6V0luYnle6gIZIxE-roziMXBRE9o3r-5EkspJx4G3jfQtzwgaZvN5X3QqJCH";

using WssClient = SimpleWeb::SocketClient<SimpleWeb::WSS>;

WssClient *client = NULL;

void wss_recv_todo(const char *msgs,int data_size)
{
    pbcar::MessageWrapper msg_proto;
    msg_proto.ParseFromArray(msgs,data_size);
    if(msg_proto.has_image_upload())
    {

    }
}

void* Thread_wss_client_send(void*) {
  sleep(5);
  
  while(1){
    auto send_stream = make_shared<WssClient::SendStream>();
    sleep(1);
    std::string message = "hello world!";
    cout << "Client: Sending message: \"" << message << "\"" << endl;
    *send_stream << message;
    if(client != NULL)
      client->connection->send(send_stream);        
  }
}

void* Thread_wss_client(void*) {

  //client = new WssClient("localhost:8181/echo", false);
  client = new WssClient("42.62.85.20:9448/ws/text", false);
  client->on_message = [](shared_ptr<WssClient::Connection> connection, shared_ptr<WssClient::Message> message) {
    auto message_str = message->string();

    cout << "Client: Message received: \"" << message_str << "\"" << endl;

    wss_recv_todo(message_str.c_str(),message_str.size());
  };

  client->on_open = [](shared_ptr<WssClient::Connection> connection) {
    cout << "Client: Opened connection" << endl;
  };

  client->on_close = [](shared_ptr<WssClient::Connection> /*connection*/, int status, const string & /*reason*/) {
    cout << "Client: Closed connection with status code " << status << endl;
  };

  // See http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html, Error Codes for error code meanings
  client->on_error = [](shared_ptr<WssClient::Connection> /*connection*/, const SimpleWeb::error_code &ec) {
    cout << "Client: Error: " << ec << ", error message: " << ec.message() << endl;
  };

  client->start();

  return 0;
}



class myClassCallback: public iot::DownloadCallback {
  public:
    virtual void Process(void *p, double dTotal, double dLoaded) {
      std::cout << "dTotal size is " <<dTotal << ". Already download " <<dLoaded<< std::endl ; 
    }
};

void Sleep() {
  std::cout <<"*********Begin Sleep 30 Seconds**************\n";
  sleep(5);
  std::cout <<"***********End Sleep 30 Seconds**************\n";
}

int main()
{
  // no need to parse response
  // std::string auth_tokens;
  // auth_tokens.assign(tenant_token);
  // // sotre token to file system.
  // std::string filePath = "./tenant_token.pems";
  // std::ofstream ofile(filePath);
  // if (ofile.is_open()) {
  //   ofile << auth_tokens;
  // }
  // ofile.close();

  pthread_attr_t attr1;
  pthread_attr_init(&attr1);
  pthread_t th1;
  pthread_create(&th1, &attr1, Thread_wss_client,0);

  pthread_attr_t attr2;
  pthread_attr_init(&attr2);
  pthread_t th2;
//  pthread_create(&th2, &attr2, Thread_wss_client_send,0);


  std::string downloadPath = "/tmp";
  std::string storePath = ".";
  std::ifstream file_stream("./tenant_token.pem");
  if(!file_stream.good())
  {
      std::cout << "not find ./tenant_token.pem" << std::endl;
      return 0;
  }
  std::stringstream buffer;
  std::ifstream in("./tenant_token.pem");
  buffer << in.rdbuf();
  std::string tenant_token = buffer.str();

  iot::OtaClient *iot_cli = new iot::OtaClient();

  iot_cli->Init(downloadPath,storePath,"");
  std::string auth_token;
  std::string id_data = "{\"device_id\": \"test_id_02\"}";
  std::string requestData = "hello world";
  std::string newTenantToken;
  int ret = iot_cli->AuthDevice(id_data, tenant_token, newTenantToken);
  if(ret == 0) {
    std::cout <<"AuthDevice Succeed!\n" << newTenantToken << std::endl;
  } else {
    std::cout <<"AuthDevice Failed!\n";    
    delete iot_cli;
    return -1;
  }

  std::string attrJson = 
  "[{\"name\":\"artifact_name\",\"value\":\"001\"},{\"name\":\"device_type\",\"value\":\"otav1_type\"}]";
  ret = iot_cli->ReportDeviceAttrs(attrJson);
  if(ret == 0) {
    std::cout <<"ReportDeviceAttrs Succeed!\n";
  } else {
    std::cout <<"ReportDeviceAttrs Failed!\n";
  }
  std::string artifactName = "001";
  std::string artifactType = "otav1_type";
  std::string localPath    = "/home/rb-xu/dev/c++/otacli-dev/build/";
  while(true) {
    iot_cli->QueryServerVersion();
    Sleep();
  }
  while(true) {
    if (ret == static_cast<int>(iot::ErrorCode::kUnauthorized)) {
      iot_cli->AuthDevice(id_data, tenant_token, newTenantToken);
    }
    myClassCallback cb;
    ret = iot_cli->QueryUpdatorPackage(artifactName, artifactType);
    if (ret == 0) {
      ret = iot_cli->DownLoadUpdatorPackage(localPath, &cb, NULL);
    } else if(ret == static_cast<int>(iot::ErrorCode::kNoContent)) {
      std::cout << "No Update Package Detected.\n";
      Sleep();
      continue;
    } else {
      std::cout << "QueryUpdatorPackage Failed.\n";
      Sleep();
      continue;
    } 
#if 1 
    if(ret == 0) {
      std::string statusJson = "{\"status\":\"downloading\"}";
      ret = iot_cli->ReportDeployStatus(statusJson);
      if (ret == 0) {
        std::cout<<"ReportDeployStatus 'success' Succeed.\n";
      } else {
        std::cout<<"ReportDeployStatus 'success' Failed.\n";
      }
    } else {
      std::string statusJson = "{\"status\":\"downloading\"}";
      ret = iot_cli->ReportDeployStatus(statusJson);
      if (ret == 0) {
        std::cout<<"ReportDeployStatus 'failed' Succeed.\n";
      } else {
        std::cout<<"ReportDeployStatus 'failed' Failed.\n";
      }
    }
#endif
    std::string logJson = "{\"messages\":[{\"level\":\"debug\",\"message\":\"debug\",\"timestamp\":\"2012-12-23T22:08:41+00:00\"}]}";
    ret = iot_cli->ReportDeployLogs(logJson);
    if (ret == 0) {
      std::cout<< "ReportDeployLogs Succeed. \n";
    } else {
      std::cout<< "ReportDeployLogs Failed \n";
    }
    std::string fullAttrsJson;
    ret = iot_cli->QueryDeviceFullAttrs(fullAttrsJson);
    if(ret == 0) {
      std::cout<<"QueryDeviceFullAttrs succeed." << fullAttrsJson<<"\n";
    } else {
      std::cout<<"QueryDeviceFullAttrs Failed.\n";
    }
    std::string singleAttrsJson;
    ret = iot_cli->QueryDeviceAttrsByAttrName("device_type", singleAttrsJson);
    if(ret == 0) {
      std::cout<<"QueryDeviceAttrsByAttrName succeed." << singleAttrsJson<<"\n";
    } else {
      std::cout<<"QueryDeviceAttrsByAttrName Failed.\n";
    }
    Sleep();
  }
  if (iot_cli) {
		delete iot_cli;
		iot_cli = NULL;
	}

  pthread_join(th1,NULL);
  pthread_join(th2,NULL);

	std::cout<<"no test suit\n";
	return 0;
}
