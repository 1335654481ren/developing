#include <string>
#include <json/json.h>

void readJson();
void writeJson();

int test_main(int argc, char** argv) {
	readJson();
	// writeJson();
	return 0;
}

void readJson() {
	using namespace std;
	std::string strValue = "{\"name\":\"json\",\"array\":[{\"cpp\":\"jsoncpp\"},{\"java\":\"jsoninjava\"},{\"php\":\"support\"}]}";

	Json::Reader reader;
	Json::Value value;


	if (reader.parse(strValue, value))
	{
		std::string out = value["name"].asString();
		std::cout <<"name"<< out << std::endl;
		std::cout <<"array"<<std::endl;
	}
}

void writeJson() {
	using namespace std;

	Json::Value root;
	Json::Value arrayObj;
	Json::Value item;

	item["cpp"] = "jsoncpp";
	item["java"] = "jsoninjava";
	item["php"] = "support";
	arrayObj.append(item);

	root["name"] = "json";
	root["array"] = item;

	root.toStyledString();
	std::string out = root.toStyledString();
	
	std::cout << "123456" << std::endl;
	std::cout << out << std::endl;
}
