package com.hobot.otacli;

public class OTACli{
	native int initOtaCli(String downloadDir, String authInfoDir);
	native int authDevice(String idData, String tenantToken);
	native int reportDeviceAttrs(String requestJson);
	native int reportDeployStatus(String statusJson);
	native int reportDeployLogs(String logsJson);
	native int queryUpdatorPackage(String artifactName, String deviceType);
	native int downLoadUpdatorPackage(  String localPath);
	native int setCallBack(OTACli instance);

	public void callbackfromCPlusPlus(String info){
		System.out.printf(info);
	}
	static {
		 System.load("/home/guoxing/dev/c++/otacli/build/lib/libotaclijnid.so");
	}
	public static void main(String[] args) {
		OTACli cli = new OTACli();
		String idData = "{\"device_id\": \"test_id_02\"}";
		String tenantToke = "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJleHAiOjE4MjgzMjI1NTksImp0aSI6Ijg3ZTBmYzE5LWQ0ZDctNGE4ZS1hZjQxLWU5N2JmNTQ2MjAzNCIsImlzcyI6Im1lbmRlci50ZW5hbnRhZG0iLCJzdWIiOiJtaWRlYUBob2JvdC5jYyIsInNjcCI6Im1lbmRlci4qIiwibWVuZGVyLnRlbmFudCI6Im1pZGVhIn0.RQIuzRYpf9wb6nf5r2n7MQz5-oo8pwwv7tk_cIlXMR_RprHI6UjX28zJBJQ_L30ixXOki6MVI6W9JJheoF33hRte4aI6YhvDDv3MLOIxMpL9F951bMuHNXniT8vk3L28olWpC6E6F2qrCj5nN52KwnRLgxWMU__9D8QVBbQD1DZN2P36veFdvO6_OFrhqL6bnXeoxoxATR5P9oXYFf6KnRh19epr9w1Zk3rX9oZu7_Dm1-SAPb-6jzccRqetKMsw10cGneowV1rV1adp5pMe14D-MQRniU_mhvcRF4KjUeqS_rtEuivdoR_D1RuIRXfg6ngwYgF8FBrs0oJjk2KXHQgAmLNKk--BZyHrI7KF2RywYFlBU3fBnGMzWWenHvz4HwsxJEI8LH6r7OrykZRfSQyf6FJk9tjK5TNQVnwwO2dWOaRTFft6-EVUiBRTAVqcYtVYCr6KXHZIavX7fHg1ZVi6XcyL4vCzRREu9qKg50YRDwqCeP0Ud2Nwr9s19gPo";
		int ret = cli.authDevice(idData, tenantToke);
		System.out.printf("ret is %d",ret);
		cli.callbackfromCPlusPlus("xdda\n");
	    ret = cli.setCallBack(cli);
		System.out.printf("ret is %d",ret);
	}
}
