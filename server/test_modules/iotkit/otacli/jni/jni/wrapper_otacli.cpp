#include "wrapper_otacli.h"
wrapper_otacli &wrapper_otacli::getInstance() {
  static wrapper_otacli otacli_;
  return otacli_;
}
int wrapper_otacli::Init(const std::string &downloadDir, const std::string &authInfoDir) {
  return client_.Init(downloadDir, authInfoDir);
}
int wrapper_otacli::AuthDevice(const std::string &id_data, const std::string &tenantToken) {
  return client_.AuthDevice(id_data, tenantToken);
}
int wrapper_otacli::ReportDeviceAttrs(const std::string &requestJson){
  return client_.ReportDeviceAttrs(requestJson);
}

int wrapper_otacli::ReportDeployStatus(const std::string &statusJson) {  
  return client_.ReportDeployStatus(statusJson);
}

int wrapper_otacli::ReportDeployLogs(const std::string &logsJson) {
  return client_.ReportDeployLogs(logsJson);
}
int wrapper_otacli::QueryUpdatorPackage(const std::string &artifactName, 
                                                const std::string &artifactType 
                                                ) {
  return client_.QueryUpdatorPackage(artifactName, artifactType);
}

int wrapper_otacli::DownLoadUpdatorPackage(
        const std::string &localPath) {
  return client_.DownLoadUpdatorPackage(localPath, NULL, NULL);
}

