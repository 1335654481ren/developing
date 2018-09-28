#include <iostream>
#include "otacli/ota_client.h"
#include "otacli/hobot_utils.h"
#include "otacli/ota_typedef.h"
#include <unistd.h>

#include "hobotlog/hobotlog.hpp"
#define TEST_TAG "hobottest"

  const char *tenant_token ="eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9."
"eyJleHAiOjE4NDE5MDM0OTAsImp0aSI6IjU2ZGQ3YWRiLWU5NjktNDhlZS04NjUyLTBmYzA1ZjBhNzAwNCIsImlzcyI6IkhvYm90Iiwic3ViIjoidGVzdDI"
"yQGhvYm90LmNjIiwic2NwIjoiaG9ib3QuKiIsImhvYm90LnRlbmFudCI6Im1pZGVhIn0.eQp2qBaoOEVjulEtxyvn1GTWBC_g-"
"_djxrLJRCdDgY9rgWMxOhLy9Wodme0hSXBOt9ObnKq8bGiTENumxsesrocHhtkBykbfJz7ko9c2KsK5ltBF3D6Ff4rU2WMdF46MzsIfmdw98PfZrGavMQAC"
"9TqfybsfKTtvDaEKtJkqg_5WCf97NHed9mYcsoq_6gUVy6nc7t3NCyenJ8f8sg-"
"cub6cOo4QOwKO2Fb4qDOyIszHwfFxSDmSKSLrgNB_Nl0VofjX6YkyV8IK8CsY26MJ93ZFTRsIIOZnqnBVbDl1J49Em-DgXaBimeq3eR25bNcKS7IQ5AV-"
"AW8JUX91bgu7gPnpuj6JGIyJ6Nk6Sk4S-w59wPUukvDFqRc67uDgyG7RnrQFMT9ihctYz22zZ56OPMbJhByqIr_gX7DP2CDEGUbzhIgNBQ-"
"kkRxn1GnvGzGEJVZR6EG0KOv6ougwhCFQPt0NhHEeoCti69nHtblIS5QoKMzFsvLjfCDMcnN7K0nY";

class myClassCallback: public iot::DownloadCallback {
  public:
    virtual void Process(void *p, double dTotal, double dLoaded) {
      std::cout << "dTotal size is " <<dTotal << ". Already download " <<dLoaded<< std::endl ; 
    }
};
void Sleep() {
  LOGI<<"*********Begin Sleep 30 Seconds**************\n";
  sleep(30);
  LOGI<<"***********End Sleep 30 Seconds**************\n";
}
int main()
{
  SetLogLevel(HOBOT_LOG_DEBUG);
  std::string downloadPath = "/tmp";
  std::string storePath = ".";
  iot::OtaClient *iot_cli = new iot::OtaClient();
  iot_cli->Init(downloadPath,storePath,"");
  std::string auth_token;
  std::string id_data = "{\"device_id\": \"test_id_02\"}";
  std::string requestData = "hello world";
  std::string newTenantToken;
  int ret = iot_cli->AuthDevice(id_data, tenant_token, newTenantToken);
  if(ret == 0) {
    LOGI<<"AuthDevice Succeed!\n";
  } else {
    LOGE<<"AuthDevice Failed!\n";    
    delete iot_cli;
    return -1;
  }
  std::string attrJson = 
  "[{\"name\":\"artifact_name\",\"value\":\"001\"},{\"name\":\"device_type\",\"value\":\"otav1_type\"}]";
  ret = iot_cli->ReportDeviceAttrs(attrJson);
  if(ret == 0) {
    LOGI<<"ReportDeviceAttrs Succeed!\n";
  } else {
    LOGE<<"ReportDeviceAttrs Failed!\n";
  }
  std::string artifactName = "001";
  std::string artifactType = "otav1_type";
  std::string localPath    = "/home/rb-xu/dev/c++/otacli-dev/build/";
  while(true) {
    if (ret == static_cast<int>(iot::ErrorCode::kUnauthorized)) {
      iot_cli->AuthDevice(id_data, tenant_token, newTenantToken);
    }
    myClassCallback cb;
    ret = iot_cli->QueryUpdatorPackage(artifactName, artifactType);
    if (ret == 0) {
      ret = iot_cli->DownLoadUpdatorPackage(localPath, &cb, NULL);
    } else if(ret == static_cast<int>(iot::ErrorCode::kNoContent)) {
      LOGI << "No Update Package Detected.\n";
      Sleep();
      continue;
    } else {
      LOGE << "QueryUpdatorPackage Failed.\n";
      Sleep();
      continue;
    } 
#if 1 
    if(ret == 0) {
      std::string statusJson = "{\"status\":\"downloading\"}";
      ret = iot_cli->ReportDeployStatus(statusJson);
      if (ret == 0) {
        LOGI<<"ReportDeployStatus 'success' Succeed.\n";
      } else {
        LOGE<<"ReportDeployStatus 'success' Failed.\n";
      }
    } else {
      std::string statusJson = "{\"status\":\"downloading\"}";
      ret = iot_cli->ReportDeployStatus(statusJson);
      if (ret == 0) {
        LOGI<<"ReportDeployStatus 'failed' Succeed.\n";
      } else {
        LOGE<<"ReportDeployStatus 'failed' Failed.\n";
      }
    }
#endif
    std::string logJson = "{\"messages\":[{\"level\":\"debug\",\"message\":\"debug\",\"timestamp\":\"2012-12-23T22:08:41+00:00\"}]}";
    ret = iot_cli->ReportDeployLogs(logJson);
    if (ret == 0) {
      LOGI<< "ReportDeployLogs Succeed. \n";
    } else {
      LOGE<< "ReportDeployLogs Failed \n";
    }
    std::string fullAttrsJson;
    ret = iot_cli->QueryDeviceFullAttrs(fullAttrsJson);
    if(ret == 0) {
      LOGI<<"QueryDeviceFullAttrs succeed." << fullAttrsJson<<"\n";
    } else {
      LOGE<<"QueryDeviceFullAttrs Failed.\n";
    }
    std::string singleAttrsJson;
    ret = iot_cli->QueryDeviceAttrsByAttrName("device_type", singleAttrsJson);
    if(ret == 0) {
      LOGI<<"QueryDeviceAttrsByAttrName succeed." << singleAttrsJson<<"\n";
    } else {
      LOGE<<"QueryDeviceAttrsByAttrName Failed.\n";
    }
    Sleep();
  }
  if (iot_cli) {
		delete iot_cli;
		iot_cli = NULL;
	}
	LOGI<<"no test suit\n";
	return 0;
}
