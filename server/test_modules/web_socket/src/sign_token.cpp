//
// Created by Tingcheng Wu on 18-01-15.
// Copyright (c) 2018 Horizon Robotics. All rights reserved.
//
#include <string.h>
#include <stdio.h>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <thread>
#include <sstream>

#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include <openssl/evp.h>  
#include <openssl/buffer.h> 
#include "sign_token.h"

int TransClient::Init(const std::string &url, const std::string &dev_json,
          const std::vector<std::string> services) {
  Json::Reader reader;
  Json::Value root;  // 'root' will contain the root value after parsing.
  if (reader.parse(dev_json, root)){
    std::cout << "Init: json parse error!" << std::endl;
    return static_cast<int>(ErrorCode::kJsonFormatError);
  }
  root["collectComments"] = false;
  root["regist_time"] = static_cast<int>(time(nullptr));
  std::string secretkey;
  auto ret = ProcKey(root, &secretkey);
  if (ret == ErrorCode::kPropertyMissing) {
    std::cout << "Init: property missing!" << std::endl;
    return static_cast<int>(ErrorCode::kJsonFormatError);
  } else if (ret != ErrorCode::kSuccess) {
    std::cout << "Init: err:" << (static_cast<int>(ret)) << std::endl;
    return static_cast<int>(ErrorCode::kSystemError);
  }
  root["key"] = secretkey;
  DeviceInfoSet(root);
  for (auto service : services) {
    root["server"].append(service);
  }
  if (HttpClient::Instance()->Init() != 0) {
    std::cout << "Init: http init error!" << std::endl;
    return static_cast<int>(ErrorCode::kSystemError);
  }
  // Http Res
  HrBuffer buf;
  constexpr int kBufLen = 1024;
  char resp[kBufLen];
  buf.buffer = resp;
  buf.buf_len = kBufLen;
  long http_code = 0;  // NOLINT
  if (0 != HttpClient::Instance()->Post(url.c_str(),
                root.toStyledString().c_str(),
                &buf, &http_code)) {
    std::cout << "Init: http curl timeout!" << std::endl;
    return static_cast<int>(ErrorCode::kTimeout);
  }
  if (reader.parse(buf.buffer, root)){
    std::cout << "Init: json parse error!"<< std::endl;
    return static_cast<int>(ErrorCode::kJsonFormatError);
  }
  if (!root.isMember("status")) {
    std::cout << "Init: Response json property missing!" << std::endl;
    return static_cast<int>(ErrorCode::kServerError);
  }
  if (root["status"].asInt() != 0) {
    std::cout << "Init: init error!" << root["msg"] << std::endl;
    return static_cast<int>(ErrorCode::kSystemError);
  }
  TokenSet(root["token"]);
  std::cout << "Init: init success!" << std::endl;
  return static_cast<int>(ErrorCode::kSuccess);
}

// base64 解码
char * TransClient::base64Decode(char *input, int length, bool newLine)
{
    BIO *b64 = NULL;
    BIO *bmem = NULL;
    char *buffer = (char *)malloc(length);
    memset(buffer, 0, length);
    b64 = BIO_new(BIO_f_base64());
    if (!newLine) {
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    }
    bmem = BIO_new_mem_buf(input, length);
    bmem = BIO_push(b64, bmem);
    BIO_read(bmem, buffer, length);
    BIO_free_all(bmem);

    return buffer;
}
// base64 编码
char * TransClient::base64Encode(const char *buffer, int length, bool newLine)
{
    BIO *bmem = NULL;
    BIO *b64 = NULL;
    BUF_MEM *bptr;

    b64 = BIO_new(BIO_f_base64());
    if (!newLine) {
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    }
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, buffer, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);
    BIO_set_close(b64, BIO_NOCLOSE);

    char *buff = (char *)malloc(bptr->length + 1);
    memcpy(buff, bptr->data, bptr->length);
    buff[bptr->length] = 0;
    BIO_free_all(b64);

    return buff;
}

std::string TransClient::sign(const char *private_key, const std::string &content)
{
    BIO *bufio = NULL;
    RSA *rsa = NULL;
    EVP_PKEY *evpKey = NULL;
    bool verify = false;
    EVP_MD_CTX ctx;
    int result = 0;
    unsigned int size = 0;
    char *sign = NULL;
    std::string signStr = "";
 
    bufio = BIO_new_mem_buf((void*)private_key, -1);
    if (bufio == NULL) {
     printf("BIO_new_mem_buf failed");
     goto safe_exit;
    }
 
    rsa = PEM_read_bio_RSAPrivateKey(bufio, NULL, NULL, NULL);
    if (rsa == NULL) {
      printf("PEM_read_bio_RSAPrivateKey failed");
      goto safe_exit;
    }
 
    evpKey = EVP_PKEY_new();
    if (evpKey == NULL) {
      printf("EVP_PKEY_new failed");
      goto safe_exit;
    }
 
    if ((result = EVP_PKEY_set1_RSA(evpKey, rsa)) != 1) {
      printf("EVP_PKEY_set1_RSA failed");
      goto safe_exit;
    }
 
    EVP_MD_CTX_init(&ctx);
 
    if (result == 1 && (result = EVP_SignInit_ex(&ctx, 
            EVP_md5(), NULL)) != 1) {
      printf("EVP_SignInit_ex failed");
    }
 
    if (result == 1 && (result = EVP_SignUpdate(&ctx, 
            content.c_str(), content.size())) != 1) {
      printf("EVP_SignUpdate failed");
    }
 
    size = EVP_PKEY_size(evpKey);
    sign = (char*)malloc(size+1);
    memset(sign, 0, size+1);
    
    if (result == 1 && (result = EVP_SignFinal(&ctx, 
            (unsigned char*)sign,
            &size, evpKey)) != 1) {
      printf("EVP_SignFinal failed");
    }
 
    if (result == 1) {
      verify = true;
    } else {
      printf("verify failed");
    }
 
    signStr = base64Encode((const char*)sign, size,false);
    EVP_MD_CTX_cleanup(&ctx);
    free(sign);
 
safe_exit:
    if (rsa != NULL) {
      RSA_free(rsa);
      rsa = NULL;
    }
 
    if (evpKey != NULL) {
      EVP_PKEY_free(evpKey);
      evpKey = NULL;
    }
 
    if (bufio != NULL) {
      BIO_free_all(bufio);
      bufio = NULL;
    }
    std::cout << signStr << std::endl;
    if(verify == true)
      printf("sign ok\n");
    return signStr;
    //return sign;
}

int TransClient::Reg(const std::string &url, const std::string &dev_json) {
  Json::Reader reader;
  Json::Value root;  // 'root' will contain the root value after parsing.
  if (!reader.parse(dev_json, root))
  {
    std::cout << "Init: json parse error!\n" <<root["tenant_token"].toStyledString() << std::endl;
    return static_cast<int>(ErrorCode::kJsonFormatError);
  }
  std::string secretkey;
  auto ret = ProcKey(root, &secretkey);
  if (ret == ErrorCode::kPropertyMissing) {
    std::cout << "Reg: property missing!" << std::endl;
    return static_cast<int>(ErrorCode::kJsonFormatError);
  } else if (ret != ErrorCode::kSuccess) {
    std::cout << "Reg: err:" << (static_cast<int>(ret)) << std::endl;
    return static_cast<int>(ErrorCode::kSystemError);
  }
  std::ifstream file_stream("./rsa_private_key.pem");
  if(!file_stream.good())
  {
      std::cout << "not find ./rsa_private_key.pem" << std::endl;
      return 0;
  }
  std::stringstream buffer;
  std::ifstream in("./rsa_private_key.pem");
  buffer << in.rdbuf();
  std::string id_rsa = buffer.str();
  std::string sing_data = sign((const char *)id_rsa.c_str(), secretkey);
  root["Sign"] = sing_data;

  DeviceInfoSet(root);
  if (HttpClient::Instance()->Init() != 0) {
    std::cout << "Reg: http init error!" << std::endl;
    return static_cast<int>(ErrorCode::kSystemError);
  }
  // Http Res
  HrBuffer buf;
  constexpr int kBufLen = 1024;
  char resp[kBufLen];
  buf.buffer = resp;
  buf.buf_len = kBufLen;
  long http_code = 0;  // NOLINT
  if (0 != HttpClient::Instance()->Post(url.c_str(),
                root.toStyledString().c_str(),
                &buf, &http_code)) {
    std::cout << "Reg: http curl timeout!" << std::endl;
    return static_cast<int>(ErrorCode::kTimeout);
  }
  if (reader.parse(buf.buffer, root)){
    std::cout << "Init: json parse error!" << std::endl;
    return static_cast<int>(ErrorCode::kJsonFormatError);
  }
  if (root.isMember("status")) {
    if (root["status"].asInt() == 0) {
      std::cout << "Reg: regist success!" << std::endl;
      return static_cast<int>(ErrorCode::kSuccess);
    } else {
      std::cout << "Reg: regist error!" << root["msg"] << std::endl;
      return static_cast<int>(ErrorCode::kSystemError);
    }
  } else {
    std::cout << "Reg: Response json property missing!" << std::endl;
    return static_cast<int>(ErrorCode::kServerError);
  }
}

int TransClient::RefreshToken(const std::string &url,
            const std::vector<std::string> services) {
  if (device_info_set_ != true) {
    std::cout << "RefreshToken: Device info unset." << std::endl;
    return static_cast<int>(ErrorCode::kPropertyMissing);
  }
  Json::Value root;  // 'root' will contain the root value after parsing.
  root = device_info_;
  root["regist_time"] = static_cast<int>(time(nullptr));
  std::string secretkey;
  auto ret = ProcKey(root, &secretkey);
  if (ret != ErrorCode::kSuccess) {
    std::cout << "RefreshToken: err:" << (static_cast<int>(ret)) << std::endl;
    return static_cast<int>(ErrorCode::kSystemError);
  }
  root["key"] = secretkey;
  for (auto service : services) {
    root["server"].append(service);
  }
  // Http Res
  HrBuffer buf;
  constexpr int kBufLen = 1024;
  char resp[kBufLen];
  buf.buffer = resp;
  buf.buf_len = kBufLen;
  long http_code = 0;  // NOLINT
  if (0 != HttpClient::Instance()->Post(url.c_str(),
                root.toStyledString().c_str(),
                &buf, &http_code)) {
    std::cout << "RefreshToken: http curl timeout!" << std::endl;
    return static_cast<int>(ErrorCode::kTimeout);
  }
  Json::Reader reader;
  if (reader.parse(buf.buffer, root)){
    std::cout << "Init: json parse error!" << std::endl;
    return static_cast<int>(ErrorCode::kJsonFormatError);
  }
  root["collectComments"] = false;
  if (!root.isMember("status")) {
    std::cout << "RefreshToken: Response json property missing!" << std::endl;
    return static_cast<int>(ErrorCode::kServerError);
  }
  if (root["status"].asInt() != 0) {
    std::cout << "RefreshToken: token error!" << root["msg"] << std::endl;
    return static_cast<int>(ErrorCode::kSystemError);
  }
  TokenSet(root["token"]);
  std::cout << "RefreshToken: token get success!" << std::endl;
  return static_cast<int>(ErrorCode::kSuccess);
}

ErrorCode TransClient::SendMeta(const std::string &url,
        const std::string meta) {
  std::string out_data = meta;
  ErrorCode ret = MakeOutputStr(&out_data);
  if (ret != ErrorCode::kSuccess) {
    return ret;
  }
  // Http Res
  HrBuffer buf;
  constexpr int kBufLen = 1024;
  char resp[kBufLen];
  buf.buffer = resp;
  buf.buf_len = kBufLen;
  long http_code = 0;  // NOLINT
  if (0 != HttpClient::Instance()->Post(url.c_str(), out_data.c_str(),
                &buf, &http_code)) {
    std::cout << "SendMeta: http curl timeout!" << std::endl;
    return ErrorCode::kTimeout;
  }
  return MetaResponseParse(buf, http_code);
}

ErrorCode TransClient::MakeOutputStr(std::string *meta) {
  return ErrorCode::kSuccess;
}