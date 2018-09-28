//
// Created by Tingcheng Wu on 18-01-15.
// Copyright (c) 2018 Horizon Robotics. All rights reserved.
//

#include <nebula_client.h>
#include <openssl/sha.h>
#include <string>
#include <vector>
#include <memory>
#include "json/json.h"

ErrorCode NebulaClient::ProcKey(const Json::Value root, std::string *key) {
  if (root.isMember("id_data") && root.isMember("pubkey")
     && root.isMember("tenant_token")) {
    std::string encr_in = root.toStyledString();
    *key = Sha256(encr_in);
    return ErrorCode::kSuccess;
  } else {
    return ErrorCode::kPropertyMissing;
  }
}

ErrorCode NebulaClient::DeviceInfoSet(const Json::Value root) {
  if (root.isMember("id_data") && root.isMember("pubkey")
     && root.isMember("tenant_token")) {
    device_info_["id_data"] = root["id_data"].asString();
    device_info_["pubkey"] = root["pubkey"].asString();
    device_info_["tenant_token"] = root["tenant_token"].asString();
    device_info_set_ = true;
    return ErrorCode::kSuccess;
  } else {
    return ErrorCode::kPropertyMissing;
  }
}

void NebulaClient::TokenSet(const Json::Value token) {
  if (token.isMember("TK")) {
    meta_token_ = token["TK"].asString();
    meta_token_set_ = true;
  }
}

std::string NebulaClient::Sha256(const std::string str) {
  std::string encr_in(str.c_str());
  unsigned char hash[SHA256_DIGEST_LENGTH+1];
  hash[SHA256_DIGEST_LENGTH] = 0;
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, encr_in.c_str(), encr_in.length());
  SHA256_Final(hash, &sha256);
  char digestout[2*SHA256_DIGEST_LENGTH + 1];
  digestout[2*SHA256_DIGEST_LENGTH] = 0;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    snprintf(&digestout[2*i], 2*SHA256_DIGEST_LENGTH, "%02x", hash[i]);
  }
  std::string encr_out(digestout);  // = "test";
  return encr_out;
}

ErrorCode NebulaClient::MakeOutputStr(std::string *meta) {
  if (meta_token_set_ == false) {
    return ErrorCode::kAuthError;
  }
  *meta = "{\"token\":\"" + meta_token_ + "\",\"content\":" + *meta + "}";
  return ErrorCode::kSuccess;
}

ErrorCode NebulaClient::MetaResponseParse(const HrBuffer buf,const long http_code) 
{  // NOLINT

  Json::Reader reader;
  Json::Value root;  // 'root' will contain the root value after parsing.
  root["collectComments"] = false; 
  if (reader.parse(buf.buffer, root)){
    std::cout << "Init: json parse error!" << std::endl;
    return ErrorCode::kJsonFormatError;
  }
  if (!root.isMember("status")) {
    std::cout << "MetaResponseParse: Response json property missing!" << std::endl;
    return ErrorCode::kServerError;
  }
  if (root["status"].asInt() == SERVER_ERR_AUTH) {
    std::cout << "MetaResponseParse: auth error:" << root["msg"] << std::endl;
    // auth fail
    meta_token_set_ = false;
    return ErrorCode::kAuthError;
  }
  if (root["status"].asInt() != 0) {
    std::cout << "MetaResponseParse: token error:" << root["msg"] << std::endl;
    return ErrorCode::kSystemError;
  }
  std::cout << "MetaResponseParse: token get success!" << std::endl;
  return ErrorCode::kSuccess;
}

int NebulaClient::StatusSend(const std::string status_json) {
  return static_cast<int> (SendMeta(kServerUrl + "/" + kStatusUrl,
                                    status_json));
}

int NebulaClient::GpsSend(const std::string gps_json) {
  return static_cast<int> (SendMeta(kServerUrl + "/" + kGpsUrl,
                                    gps_json));
}

int NebulaClient::EventSend(const std::string event_json) {
  return static_cast<int> (SendMeta(kServerUrl + "/" + kEventUrl,
                                    event_json));
}

int NebulaClient::EventFileSend(const std::string eventfile_json) {
  return static_cast<int> (SendMeta(kServerUrl + "/" + kEventFileUrl,
                                    eventfile_json));
}



int NebulaClient::ShowTokenStatus() {
  if (meta_token_set_) {
    std::cout << "Meta token setted." << std::endl;
  }
  return 0;
}

int NebulaClient::RefreshToken() {
  std::vector<std::string> service = {"TK"};
  auto ret = Init(kServerUrl + "/" + kTvmUrl, dev_json_, service);
  if (ret) {
    std::cout << "InitClient Err!" << ret << std::endl;
    return -1;
  }
  return 0;
}

int NebulaClient::RegistDevice(const std::string &devinfo_json)
{
  return Reg(kServerUrl + "/" + kRegUrl, devinfo_json);
}

