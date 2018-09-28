//
// Created by guoxing xu on 18-03-09.
// Copyright (c) 2018 Horizon Robotics. All rights reserved.
// iot OTA related APIs
//
#include "otacli/ota_client.h"

#include <string.h>
#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <vector>
#include "curl/curl.h"
#include "hobotlog/hobotlog.hpp"
#include "json/json.h"

#include "otacli/hobot_utils.h"
#include "otacli/http_client.h"

#define DEFAULT_KEYSIZE 2048
iot::OtaClient::OtaClient() { initd_ = false; }

int iot::OtaClient::Init(const std::string &downLoadDir,
                         const std::string &authInfoStoreDir,
                         const std::string &baseUrl) {
  authfile_path_ = authInfoStoreDir;
  if (authfile_path_.back() != '/') {
    authfile_path_.push_back('/');
  }
  if (iot::HobotUtils::FileExists(authfile_path_) == false) {
    LOGE << "Not Valid file path";
    return static_cast<int>(ErrorCode::kInitError);
  }

  download_path_ = authInfoStoreDir;
  if (download_path_.back() != '/') {
    download_path_.push_back('/');
  }
  if (iot::HobotUtils::FileExists(download_path_) == false) {
    LOGE << "Not Valid file path";
    return static_cast<int>(ErrorCode::kInitError);
  }
  if (baseUrl.length() == 0) {
    base_url_ = kBaseUrl;
  } else {
    base_url_ = baseUrl;
  }
  // do only once
  SetLogLevel(HOBOT_LOG_DEBUG);
  curl_global_init(CURL_GLOBAL_DEFAULT);
  initd_ = true;
  return static_cast<int>(ErrorCode::kSuccess);
}

iot::OtaClient::~OtaClient() { curl_global_cleanup(); }

int iot::OtaClient::AuthDevice(const std::string &idData,
                               const std::string &tenantToken,
                               std::string &newTenantToken) {
  if (initd_ == false) {
    return static_cast<int>(ErrorCode::kInitError);
  }
  int ret = 0;
  std::string priKey, pubKey;
  if ((ret = GetRsaKeyPair(idData, priKey, pubKey)) != 0) {
    LOGE << "Internal Error.";
    return ret;
  }
  std::string authToken;
  return AuthDevice(idData, pubKey, priKey, tenantToken, authToken,
                    newTenantToken);
}

int iot::OtaClient::ReportDeviceAttrs(const std::string &requestJson) {
  if (initd_ == false) {
    return static_cast<int>(ErrorCode::kInitError);
  }

  std::string errs;
  LOGD << "IN C: ReportDeviceAttrs" << requestJson;
  if (auth_token_.length() == 0) {
    LOGE << "ReportDeviceAttrs: no token!";
    return static_cast<int>(ErrorCode::kFailed);
  }
  if (VerifyJsonString(requestJson, errs) == false) {
    LOGE << "ReportDeviceAttrs: json parse error!" << requestJson << ":"
         << errs;
    return static_cast<int>(ErrorCode::kJsonFormatError);
  }

  HttpClient client;
  std::string resp;
  long status_code = 0;
  std::vector<std::string> curl_headers;
  std::string curl_header = "Authorization: Bearer " + auth_token_;
  curl_headers.push_back("Content-Type: application/json");
  curl_headers.push_back(curl_header);
  std::string url = base_url_ + kAttrs;
  client.set_curl_headers(curl_headers);
  int ret = client.HttpsPatch(url, requestJson, kCaPath, resp, &status_code);
  if (ret == 0 && status_code == 401) {
    return static_cast<int>(ErrorCode::kUnauthorized);
  }
  if (ret != 0 || status_code != 200) {
    LOGE << resp.c_str();
    return static_cast<int>(ErrorCode::kFailed);
  }
  return static_cast<int>(ErrorCode::kSuccess);
}

int iot::OtaClient::QueryDeviceFullAttrs(std::string &responseJson) {
  if (initd_ == false) {
    return static_cast<int>(ErrorCode::kInitError);
  }
  HttpClient client;
  long status_code;
  std::string resp;
  std::vector<std::string> curl_headers;
  std::string curl_header = "Authorization: Bearer " + auth_token_;
  curl_headers.push_back("Content-Type: application/json");
  curl_headers.push_back(curl_header);
  std::string url = base_url_ + kAttrs;
  client.set_curl_headers(curl_headers);
  int ret = client.HttpsGet(url, kCaPath, resp, &status_code);
  if (ret == 0 && status_code == 401) {
    return static_cast<int>(ErrorCode::kUnauthorized);
  }
  if (ret == 0 && status_code == 200) {
    responseJson.assign(resp.c_str());
    return static_cast<int>(ErrorCode::kSuccess);
  }
  if (ret == 0 && status_code == 204) {
    return static_cast<int>(ErrorCode::kNoContent);
  }
  return static_cast<int>(ErrorCode::kFailed);
}

int iot::OtaClient::QueryDeviceAttrsByAttrName(const std::string &key,
                                               std::string &responseJson) {
  if (initd_ == false) {
    return static_cast<int>(ErrorCode::kInitError);
  }
  HttpClient client;
  long status_code = 0;
  std::string resp;
  std::vector<std::string> curl_headers;
  std::string curl_header = "Authorization: Bearer " + auth_token_;
  curl_headers.push_back("Content-Type: application/json");
  curl_headers.push_back(curl_header);
  std::string url = base_url_ + kAttrs + "/" + key;
  client.set_curl_headers(curl_headers);
  int ret = client.HttpsGet(url, kCaPath, resp, &status_code);
  if (ret == 0 && status_code == 401) {
    return static_cast<int>(ErrorCode::kUnauthorized);
  }
  if (ret == 0 && status_code == 204) {
    return static_cast<int>(ErrorCode::kNoContent);
  }
  if (ret == 0 && status_code == 200) {
    responseJson.assign(resp.c_str());
    return static_cast<int>(ErrorCode::kSuccess);
  }
  LOGE << resp.c_str();
  return static_cast<int>(ErrorCode::kFailed);
}

int iot::OtaClient::ReportDeployStatus(const std::string &statusJson) {
  if (initd_ == false) {
    return static_cast<int>(ErrorCode::kInitError);
  }

  std::string errs;
  if (VerifyJsonString(statusJson, errs) == false) {
    LOGD << "Reg: json parse error!" << statusJson << ":" << errs;
    return static_cast<int>(ErrorCode::kJsonFormatError);
  }

  HttpClient client;
  std::string resp;

  long status_code = 0;
  std::vector<std::string> curl_headers;
  std::string curl_header = "Authorization: Bearer " + auth_token_;
  curl_headers.push_back("Content-Type: application/json");
  curl_headers.push_back(curl_header);

  std::string url = base_url_ + kDeployBaseUrl + device_id_ + kStatus;
  client.set_curl_headers(curl_headers);
  int ret = client.HttpsPut(url, statusJson, kCaPath, resp, &status_code);
  if (ret == 0 && status_code == 204) {
    return static_cast<int>(ErrorCode::kSuccess);
  }
  if (status_code == 401) {
    return static_cast<int>(ErrorCode::kUnauthorized);
  }
  LOGE << resp.c_str();
  return static_cast<int>(ErrorCode::kFailed);
}

int iot::OtaClient::ReportDeployLogs(const std::string &logsJson) {
  if (initd_ == false) {
    return static_cast<int>(ErrorCode::kInitError);
  }

  std::string errs;
  if (VerifyJsonString(logsJson, errs) == false) {
    LOGD << "Reg: json parse error!" << errs;
    return static_cast<int>(ErrorCode::kJsonFormatError);
  }

  HttpClient client;
  std::string resp;
  long status_code = 0;
  std::vector<std::string> curl_headers;
  std::string curl_header = "Authorization: Bearer " + auth_token_;
  curl_headers.push_back("Content-Type: application/json");
  curl_headers.push_back(curl_header);
  client.set_curl_headers(curl_headers);

  std::string url = base_url_ + kDeployBaseUrl + device_id_ + kLogs;
  LOGD << "ReportDeployLogs" << url;
  int ret = client.HttpsPut(url, logsJson, kCaPath, resp, &status_code);
  if (ret == 0 && (status_code == 204 || status_code == 200)) {
    return static_cast<int>(ErrorCode::kSuccess);
  }
  if (status_code == 401) {
    return static_cast<int>(ErrorCode::kUnauthorized);
  }
  LOGE << resp.c_str();
  return static_cast<int>(ErrorCode::kFailed);
}

int iot::OtaClient::QueryUpdatorPackage(const std::string &artifactName,
                                        const std::string &artifactType) {
  if (initd_ == false) {
    return static_cast<int>(ErrorCode::kInitError);
  }
  download_url_.clear();
  HttpClient client;
  long status_code = 0;
  std::string resp;
  std::vector<std::string> curl_headers;
  std::string curl_header = "Authorization: Bearer " + auth_token_;
  curl_headers.push_back(curl_header);
  client.set_curl_headers(curl_headers);

  std::string url = base_url_ + kDeployBaseUrl + kUpdator + "?" +
                    "artifact_name=" + artifactName + "&" + "device_type=" +
                    artifactType;
  int ret = client.HttpsGet(url, kCaPath, resp, &status_code);
  if (ret == 0 && status_code == 401) {
    return static_cast<int>(ErrorCode::kUnauthorized);
  }
  if (ret == 0 && status_code == 204) {
    return static_cast<int>(ErrorCode::kNoContent);
  }
  if (ret != 0) {
    LOGE << "HttpsGet Failed. ret = " << ret << ":" << resp.c_str();
    return static_cast<int>(ErrorCode::kFailed);
  }
  LOGD << url << ":" << resp.c_str() << resp.length();
  // parse updatorpackageUrl
  std::string errs;
  Json::CharReaderBuilder builder;
  builder["collectComments"] = false;
  std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
  Json::Value root;  // 'root' will contain the root value after parsing.
  if (!reader->parse(resp.c_str(), resp.c_str() + resp.length(), &root,
                     &errs)) {
    LOGE << "Parse download url failed.";
    LOGE << resp.c_str();
    return static_cast<int>(ErrorCode::kJsonFormatError);
  }
  if (root.isMember("artifact") && root["artifact"].isMember("source") &&
      root["artifact"]["source"].isMember("uri")) {
    download_url_ = root["artifact"]["source"]["uri"].asCString();
  } else {
    LOGE << "Response json property missing `uri`";
    return static_cast<int>(ErrorCode::kServerError);
  }

  if (root.isMember("id")) {
    device_id_ = root["id"].asCString();
    LOGD << device_id_;
  } else {
    LOGE << "Response json property missing `id`";
    return static_cast<int>(ErrorCode::kServerError);
  }
  return static_cast<int>(ErrorCode::kSuccess);
}

int iot::OtaClient::DownLoadUpdatorPackage(const std::string &localPath,
                                           DownloadCallback *cb, void *pParam) {
  if (initd_ == false) {
    return static_cast<int>(ErrorCode::kInitError);
  }
  if (download_url_.length() == 0) {
    LOGE << "No Valid Download.";
    return static_cast<int>(ErrorCode::kFailed);
  }
#if 0
  HttpClient client;
  client.set_process_cb(cb);
  client.set_process_cb_param(pParam);
  int  ret = client.HttpDownload(download_url_, tmpPath);
  if (ret != 0) {
    LOGE <<"DownLoad Failed." <<ret;
    return ret;
  }
  LOGD <<"DownLoad Succeed." <<ret;
#endif
#ifdef LIBTARGZ
  // 定制格式tar.gz,包含有检验信息
  LOGD << download_url_ << ":" << localPath;
  std::string tmpFile;
  std::string tmpFileFloder;
  int result = 0;
  if (download_path_.back() != '/') {
    tmpFile = download_path_ + "/tmp.tar.gz";
    tmpFileFloder = download_path_ + "/tmp/";
  } else {
    tmpFile = download_path_ + "tmp.tar.gz";
    tmpFileFloder = download_path_ + "tmp/";
  }
  HttpClient client;
  client.set_process_cb(cb);
  client.set_process_cb_param(pParam);
  int ret = client.HttpDownload(download_url_, tmpFile);
  if (ret != 0) {
    LOGE << "DownLoad Failed." << ret;
    return ret;
  }

  LOGE << "DownLoad Succeed." << ret;

  // TBD GUOXING.XU@HOBOT.CC
  // 1 do unpack downlaod file
  std::string realFile;
  std::map<std::string, std::string> fileMd5Map;
  {
    if (iot::HobotUtils::ExtraTarDotGzFile(tmpFile, tmpFileFloder) != 0) {
      LOGE << "ExtraTarDotGzFile failed. " << tmpFile;
      result = static_cast<int>(ErrorCode::kFileOPError);
      goto end;
    }
    std::string manifest = tmpFileFloder + "manifest";
    std::ifstream infile(manifest);
    if (infile.good()) {
      std::string line;
      std::stringstream str;
      std::string file, hash;
      while (getline(infile, line)) {
        str.clear();
        str.str(line.c_str());
        str >> hash >> file;
        fileMd5Map[file] = hash;
        if (realFile.length() == 0) {
          realFile = file;
        }
      }
    } else {
      LOGE << "read manifest failed. ";
      result = static_cast<int>(ErrorCode::kFileOPError);
      goto end;
    }
    infile.close();
  }
  // 2 dealwith header.tar.gz
  // 2.1check sha
  {
    std::string header = tmpFileFloder + "header.tar.gz";
    std::string headerSHA2;
    iot::HobotUtils::ComputeSHA256(header, headerSHA2);
    if (headerSHA2.compare(fileMd5Map["header.tar.gz"]) != 0) {
      LOGE << "file is corrupted. ";
      LOGE << headerSHA2 << ":" << fileMd5Map["header.tar.gz"];
      result = static_cast<int>(ErrorCode::kServerError);
      goto end;
    }
    // 2.2uppack header.tar.gz
    if (iot::HobotUtils::ExtraTarDotGzFile(header, tmpFileFloder) != 0) {
      result = static_cast<int>(ErrorCode::kFileOPError);
      goto end;
    }
  }
  // 3 dealwith data/0000.tar.gz
  // 3.1unpack
  {
    std::string data = tmpFileFloder + "data/0000.tar.gz";
    std::string dataFloder = tmpFileFloder + "/data/0000/";
    if (iot::HobotUtils::ExtraTarDotGzFile(data, dataFloder) != 0) {
      LOGE << "ExtraTarDotGzFile failed. " << data;
      result = static_cast<int>(ErrorCode::kFileOPError);
      goto end;
    }
    // 3.2check data item.
    std::string destFile = tmpFileFloder + realFile;
    // 3.3check sha
    std::string dataSHA2;
    iot::HobotUtils::ComputeSHA256(destFile, dataSHA2);
    if (dataSHA2.compare(fileMd5Map[realFile]) != 0) {
      LOGE << "file is incorrupted. ";
      LOGE << dataSHA2 << ":" << fileMd5Map[realFile];
      result = static_cast<int>(ErrorCode::kServerError);
      goto end;
    }
    LOGD << destFile;
    std::string pureName;
    std::string targetFileName;
    if (iot::HobotUtils::DirExists(localPath)) {
      if (iot::HobotUtils::GetPureFileName(realFile, pureName) == 0) {
        if (localPath.back() == '/') {
          targetFileName = localPath + pureName;
        } else {
          targetFileName = localPath + "/" + pureName;
        }
        LOGI << targetFileName;
      } else {
        result = static_cast<int>(ErrorCode::kFileOPError);
        goto end;
      }
    } else {
      targetFileName = localPath;
    }
    if (iot::HobotUtils::MoveFile(destFile, targetFileName) != 0) {
      result = static_cast<int>(ErrorCode::kFileOPError);
      goto end;
    }
    result = static_cast<int>(ErrorCode::kSuccess);
  }
#endif
end:
  // 4. clean tmp file;
  {
    iot::HobotUtils::DeleteFile(tmpFile);
    iot::HobotUtils::DeleteFloder(tmpFileFloder);
  }
  return result;
}

bool iot::OtaClient::VerifyJsonString(const std::string &jsonString,
                                      std::string &errs) {
  Json::CharReaderBuilder builder;
  builder["collectComments"] = false;
  std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
  Json::Value root;  // 'root' will contain the root value after parsing.
  if (!reader->parse(jsonString.c_str(),
                     jsonString.c_str() + jsonString.length(), &root, &errs)) {
    return false;
  }
  return true;
}

int iot::OtaClient::AuthDevice(const std::string &id_data,
                               const std::string &pubkey,
                               const std::string &prikey,
                               const std::string &tenant_token,
                               std::string &auth_token,
                               std::string &newTenantToken) {
  Json::StreamWriterBuilder wbuilder;
  Json::Value root;
  root["id_data"] = id_data;
  root["pubkey"] = pubkey;
  root["tenant_token"] = tenant_token;

  std::string request_data = Json::writeString(wbuilder, root);
  std::string signature;
  int ret = iot::HobotUtils::SignData(prikey, id_data, request_data, "SHA256",
                                      signature);
  if (ret != 0) {
    LOGE << "SignData Failed";
    return ret;
  }

  HttpClient client;
  std::string resp;
  long status_code = 0;
  std::vector<std::string> curl_headers;
  std::string curl_header = "x-men-signature: " + signature;
  curl_headers.push_back("Content-Type: application/json");
  curl_headers.push_back(curl_header);
  std::string url = base_url_ + kAuth;
  client.set_curl_headers(curl_headers);
  LOGD << "url:" << url << "\n";
  ret = client.HttpsPost(url, request_data, kCaPath, resp, &status_code);
  if (ret != 0 || (status_code != 200 && status_code != 403)) {
    LOGE << "ret = " << ret << ". status_code = " << status_code << "\n";
    return ret;
  }
  // rebind.
  if (status_code == 403) {
    // parse status_code
    std::string errs;
    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    Json::Value root;  // 'root' will contain the root value after parsing.
    if (!reader->parse(resp.c_str(), resp.c_str() + resp.length(), &root,
                       &errs)) {
      LOGE << "auth resp failed.";
      LOGE << resp.c_str();
      return static_cast<int>(ErrorCode::kJsonFormatError);
    }
    if (root.isMember("tenant_token") && root.isMember("tenant_rebinding")) {
      newTenantToken = root["tenant_token"].asCString();
      bool tenant_rebinding = root["tenant_rebinding"].asBool();
      if (tenant_rebinding) {
        return AuthDevice(id_data, pubkey, prikey, newTenantToken, auth_token,
                          newTenantToken);
      } else {
        LOGE << "Response json property missing `tenant_rebinding`";
        return static_cast<int>(ErrorCode::kServerError);
      }
    } else {
      LOGE << "Response json property missing `tenant_token`";
      return static_cast<int>(ErrorCode::kServerError);
    }
  }
  // no need to parse response
  auth_token.assign(resp.c_str());
  auth_token_ = auth_token;
  // sotre token to file system.
  std::string filePath = authfile_path_ + "token.data";
  std::ofstream ofile(filePath);
  if (ofile.is_open()) {
    ofile << auth_token_;
  }
  ofile.close();
  return 0;
}

int iot::OtaClient::GenRsaKeyPair(const std::string &encKey, int nbits) {
  if (nbits != 1024 && nbits != 2048 && nbits != 4096) {
    return static_cast<int>(ErrorCode::kParamError);
  }

  // store key to disk.
  std::string priKeyPath = authfile_path_ + kPriKeyPath;
  std::string pubKeyPath = authfile_path_ + kPubKeyPath;

  return iot::HobotUtils::GenRsaKeyPair(priKeyPath, pubKeyPath, encKey, nbits);
}

int iot::OtaClient::GetRsaKeyPair(const std::string &encKey,
                                  std::string &priKey, std::string &pubKey) {
  std::string priKeyPath = authfile_path_ + kPriKeyPath;
  std::string pubKeyPath = authfile_path_ + kPubKeyPath;
  if (iot::HobotUtils::FileExists(priKeyPath) == false ||
      iot::HobotUtils::FileExists(pubKeyPath) == false) {
    if (GenRsaKeyPair(encKey, DEFAULT_KEYSIZE) != 0) {
      return static_cast<int>(ErrorCode::kFailed);
    }
  }
  return (iot::HobotUtils::LoadKeyFromFile(priKeyPath, priKey) +
          iot::HobotUtils::LoadKeyFromFile(pubKeyPath, pubKey));
}