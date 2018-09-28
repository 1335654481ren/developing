#include "client_wss.hpp"
#include "opencv2/opencv.hpp"                                                   
#include <string>
#include <vector>
#include <pthread.h>
using namespace std;
using namespace cv;
using WssClient = SimpleWeb::SocketClient<SimpleWeb::WSS>;
VideoCapture *capture = NULL;
WssClient *client = NULL;
void* ThreadRecieve(void*) {
  sleep(5);
  while(1){
    Mat frame;
    *capture >>frame;
    imshow("hhh",frame);
    waitKey(30);
   // frame=cv::imread("1.png");
    string message = "";
    cout << "Client: Sending message: \"" << message << "\"" << endl;
    std::vector<uchar> vecImg;  // Mat 图片数据转换为vector<uchar>
    std::vector<int> vecCompression_params;
    vecCompression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    vecCompression_params.push_back(50);
    cv::imencode(".jpg", frame, vecImg, vecCompression_params);
    cout << "vecimg size="<< vecImg.size()<< endl;

    for(int i=0;i<vecImg.size();++i)
    {
    	message+=vecImg[i];
    }

    auto send_stream = make_shared<WssClient::SendStream>();
    *send_stream << message;
    if(client != NULL)
      client->connection->send(send_stream);        
  }

}

int main() {

  // Example 4: Client communication with server
  // Second Client() parameter set to false: no certificate verification
  // Possible output:
  //   Server: Opened connection 0x7fcf21600380
  //   Client: Opened connection
  //   Client: Sending message: "Hello"
  //   Server: Message received: "Hello" from 0x7fcf21600380
  //   Server: Sending message "Hello" to 0x7fcf21600380
  //   Client: Message received: "Hello"
  //   Client: Sending close connection
  //   Server: Closed connection 0x7fcf21600380 with status code 1000
  //   Client: Closed connection with status code 1000
  client = new WssClient("localhost:8181/echo", false);

  capture = new VideoCapture(0);


  client->on_message = [](shared_ptr<WssClient::Connection> connection, shared_ptr<WssClient::Message> message) {
    auto message_str = message->string();

    cout << "Client: Message received: \"" << message_str << "\"" << endl;

    cout << "Client: Sending close connection" << endl;
    //connection->send_close(1000);
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
  pthread_attr_t thread_attr;
  pthread_attr_init(&thread_attr);

  pthread_t th;
  pthread_create(&th, &thread_attr, ThreadRecieve,0);

  client->start();

  pthread_join(th,NULL);
}
