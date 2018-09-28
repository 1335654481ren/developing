//
// Created by Tingcheng Wu on 18-01-15.
// Copyright (c) 2018 Horizon Robotics. All rights reserved.
//

#ifndef NEBULA_CLIENT_H_
#define NEBULA_CLIENT_H_

#include <string>
#include "json/json.h"
#include "sign_token.h"

#define SERVER_ERR_AUTH -1

class NebulaClient : public TransClient {
 public:
  //  RegistDevice
  //  @param devinfo_json, device info json string.
  //         include property: serial_num, imei, install_id, install_time
  //  @output 0 - success
  int RegistDevice(const std::string &devinfo_json);
  //  Refresh meta update token when token is expired
  //  @output 0 - success
  int RefreshToken();
  //  Refresh S3 temporary credentials when credential is expired
  //  @output 0 - success
  int ShowTokenStatus();
  //  send status meta data to database
  //  @param status_json, status meta data in json
  //  @output 0 - success, ErrorCode: -2 - AuthError
  int StatusSend(const std::string status_json);
  //  send GPS meta data to database
  //  @param gps_json, gps meta data in json
  //  @output 0 - success, ErrorCode: -2 - AuthError
  int GpsSend(const std::string gps_json);
  //  send event meta data to database
  //  @param event_json, event meta data in json
  //  @output 0 - success, ErrorCode: -2 - AuthError
  int EventSend(const std::string event_json);
  //  send event file meta data to database
  //  @param eventfile_json, file of event meta data in json
  //  @output 0 - success, ErrorCode: -2 - AuthError
  int EventFileSend(const std::string eventfile_json);

 private:
  ErrorCode ProcKey(const Json::Value root, std::string *key) override;
  ErrorCode DeviceInfoSet(const Json::Value root) override;
  ErrorCode MakeOutputStr(std::string *meta) override;
  ErrorCode MetaResponseParse(const HrBuffer buf,
                              const long http_code) override;  // NOLINT
  void TokenSet(const Json::Value token) override;
  std::string Sha256(const std::string str);

  // -------------------------------------------------------
  // encrypt key of client
  std::string encrypt_key_ = "aTESTencrypKEY";
  // name of bucket on AWS S3
  const std::string kBucket = "s3test-1.hobot.cc";
  // address of meta server
  const std::string kServerUrl = "https://10.10.10.105:8118";
  // All of the key value above can be assigned by OP group of cloudplatform
  // -------------------------------------------------------
  std::string meta_token_ = "";
  bool meta_token_set_ = false;
  std::string dev_json_ = "";
  const std::string kRegUrl = "sn";
  const std::string kTvmUrl = "tv";
  const std::string kStatusUrl = "st";
  const std::string kGpsUrl = "gs";
  const std::string kEventUrl = "ev";
  const std::string kEventFileUrl = "fl";
};
#endif  // NEBULA_CLIENT_H_
