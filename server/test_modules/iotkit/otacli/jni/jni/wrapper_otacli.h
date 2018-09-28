#ifndef _JAVA_JNI_WRAPPER_OTACLI_H
#define _JAVA_JIN_WRAPPER_OTACLI_H
#include "ota_typedef.h"
#include "ota_client.h"
class wrapper_otacli{
 public:
  static wrapper_otacli& getInstance();
  ~ wrapper_otacli() {
  }
  int Init(const std::string &downloadDir, const std::string &authInfoDir);
  int AuthDevice(const std::string &id_data, const std::string &tenantToken);
  int ReportDeviceAttrs(const std::string &requestJson);
  int ReportDeployStatus(const std::string &statusJson);
  int ReportDeployLogs(const std::string &logsJson);
  int QueryUpdatorPackage(const std::string &artifactName, 
                                 const std::string &artifactType);
  int DownLoadUpdatorPackage(const std::string &localPath);
 private:
  wrapper_otacli() {
  }
  wrapper_otacli(const wrapper_otacli &r) = delete;
  wrapper_otacli & operator=(const wrapper_otacli&) = delete;
 private:
  iot::OtaClient client_;  
};
#endif //_JAVA_JIN_WRAPPER_OTACLI_H
