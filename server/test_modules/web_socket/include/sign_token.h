//
// Created by Tingcheng Wu on 18-01-15.
// Copyright (c) 2018 Horizon Robotics. All rights reserved.
//

#ifndef SIGN_TOKRN_H_
#define SIGN_TOKRN_H_

#include <map>
#include <string>
#include <vector>
#include "json/json.h"
#include "http_client.h"

enum class ErrorCode {
  kSuccess = 0,
  kTimeout,
  kAuthError,
  kSystemError,
  kJsonFormatError,
  kPropertyMissing,
  kServerError
};

class TransClient {
 public:
  //int Init(const std::string &url);
  //int Init(const std::string &url, const std::string &dev_json);
  //int Init(const std::string &url,
           //const std::map<std::string, std::string> &dev_desc);
  int Init(const std::string &url,
           const std::string &dev_json,
           const std::vector<std::string> services);
  int Reg(const std::string &url, const std::string &dev_json);
  int RefreshToken(const std::string &url);
  int RefreshToken(const std::string &url,
                   const std::vector<std::string> services);
  std::string sign(const char *private_key, const std::string &content);
  // base64 解码
  char * base64Decode(char *input, int length, bool newLine);
  // base64 编码
  char * base64Encode(const char *buffer, int length, bool newLine);
 protected:
  Json::Value device_info_;
  bool device_info_set_ = false;
  ErrorCode SendMeta(const std::string &url, const std::string meta);

 private:
  virtual ErrorCode MakeOutputStr(std::string *meta);
  virtual ErrorCode MetaResponseParse(const HrBuffer buf,
                                      const long http_code) = 0;  // NOLINT
  // Process encrypt key from objects in root
  virtual ErrorCode ProcKey(const Json::Value root, std::string *key) = 0;
  // Save device info in root to device_info_
  virtual ErrorCode DeviceInfoSet(const Json::Value root) = 0;
  // Save device token in response to token
  virtual void TokenSet(const Json::Value token) = 0;
};
#endif  // SIGN_TOKRN_H_