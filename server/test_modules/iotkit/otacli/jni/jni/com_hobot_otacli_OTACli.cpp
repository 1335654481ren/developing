#include "com_hobot_otacli_OTACli.h"
#include <string>
#include "wrapper_otacli.h"

JNIEXPORT jint JNICALL Java_com_hobot_otacli_OTACli_initOtaCli
  (JNIEnv *env, jobject jobj, jstring jdownloadDir, jstring jauthInfoDir) {
  const char *downloadDir = env->GetStringUTFChars(jdownloadDir, 0);
  const char *authInfoDir = env->GetStringUTFChars(jauthInfoDir, 0);
  int ret = wrapper_otacli::getInstance().Init(downloadDir, authInfoDir);
  env->ReleaseStringUTFChars(jdownloadDir, downloadDir);
  env->ReleaseStringUTFChars(jauthInfoDir, authInfoDir);
  return ret;
}
JNIEXPORT jint JNICALL Java_com_hobot_otacli_OTACli_authDevice
  (JNIEnv *env, jobject jobj, jstring jidData, jstring jtentToken) {
  const char *idData = env->GetStringUTFChars(jidData, 0);
  const char *tenantToken = env->GetStringUTFChars(jtentToken, 0);
  int ret = wrapper_otacli::getInstance().AuthDevice(idData, tenantToken);
  env->ReleaseStringUTFChars(jidData, idData);  
  env->ReleaseStringUTFChars(jtentToken, tenantToken);
  return ret;
}

JNIEXPORT jint JNICALL Java_com_hobot_otacli_OTACli_reportDeviceAttrs
  (JNIEnv *env, jobject jobj, jstring jrequestJson ) {
  const char *requestJson = env->GetStringUTFChars(jrequestJson, 0);
  int ret = wrapper_otacli::getInstance().ReportDeviceAttrs(requestJson);
  env->ReleaseStringUTFChars(jrequestJson, requestJson);
  return ret;
}

JNIEXPORT jint JNICALL Java_com_hobot_otacli_OTACli_reportDeployStatus
  (JNIEnv *env, jobject jobj, jstring jstatusJson) {
  const char *statusJson = env->GetStringUTFChars(jstatusJson, 0);
  int ret = wrapper_otacli::getInstance().ReportDeployStatus(statusJson);
  env->ReleaseStringUTFChars(jstatusJson, statusJson);
  return ret;
}

JNIEXPORT jint JNICALL Java_com_hobot_otacli_OTACli_reportDeployLogs
  (JNIEnv *env, jobject jobj, jstring jlogsJson) {
  const char *logsJson = env->GetStringUTFChars(jlogsJson, 0);
  int ret = wrapper_otacli::getInstance().ReportDeployLogs(logsJson);
  env->ReleaseStringUTFChars(jlogsJson, logsJson);
  return ret;
}

JNIEXPORT jint JNICALL Java_com_hobot_otacli_OTACli_queryUpdatorPackage
  (JNIEnv *env, jobject jobj, jstring jartifactName, jstring jartifactType) {
  const char *artifactName = env->GetStringUTFChars(jartifactName, 0);
  const char *artifactType = env->GetStringUTFChars(jartifactType, 0);
  int ret = wrapper_otacli::getInstance().QueryUpdatorPackage(artifactName, artifactType);
  env->ReleaseStringUTFChars(jartifactName, artifactName);
  env->ReleaseStringUTFChars(jartifactType, artifactType);
  return ret;
}

JNIEXPORT jint JNICALL Java_com_hobot_otacli_OTACli_downLoadUpdatorPackage
  (JNIEnv *env, jobject jobj, jstring jlocalPath) {
  const char *localPath = env->GetStringUTFChars(jlocalPath, 0);
  int ret = wrapper_otacli::getInstance().DownLoadUpdatorPackage(localPath);
  env->ReleaseStringUTFChars(jlocalPath, localPath);
  return ret;
}
JNIEXPORT jint JNICALL Java_com_hobot_otacli_OTACli_setCallBack
  (JNIEnv *env, jobject jobj, jobject jinstance) {
  jclass cls = env->GetObjectClass(jinstance);
  //jclass cls = env->FindClass("com/hobot/otacli/OTACli");
  if(cls == NULL)
  {
	  return -1;
  }
  
  jmethodID jmid = env->GetMethodID( cls, "callbackfromCPlusPlus","(Ljava/lang/String;)V");
  if(jmid ==NULL)
  {
	  return -2;
  }
  jstring info = env->NewStringUTF("this is a test log in c");
   
  env->CallVoidMethod(jinstance,jmid,info);
  
  env->ReleaseStringUTFChars(info,env->GetStringUTFChars(info,0)); 
  
  return 0;
}
