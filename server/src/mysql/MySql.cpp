#include "MySql.hpp"
#include "MySqlException.hpp"

#include <cassert>
#include <cstdint>
#include <mysql/mysql.h>

#include <string>
#include <sstream>
#include <vector>
#include <tuple>
#include <stdio.h>
#include <stdlib.h>

using std::string;
using std::vector;
using std::tuple;

MySql::MySql(
    const char* hostname,
    const char* username,
    const char* password,
    const uint16_t port
)
#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)
    : MySql(hostname, username, password, nullptr, port)
{
}
#else
    : connection_(mysql_init(nullptr))
{
    if (nullptr == connection_) {
        throw MySqlException("Unable to connect to MySQL");
    }
    const MYSQL* const success = mysql_real_connect(
        connection_,
        hostname,
        username,
        password,
        nullptr,
        port,
        nullptr,
        0);
    if (nullptr == success) {
        MySqlException mse(connection_);
        mysql_close(connection_);
        throw mse;
    }
}
#endif


MySql::MySql(
    const char* const hostname,
    const char* const username,
    const char* const password,
    const char* const database,
    const uint16_t port
)
    : connection_(mysql_init(nullptr))
{
    if (nullptr == connection_) {
        throw MySqlException("Unable to connect to MySQL");
    }
    const MYSQL* const success = mysql_real_connect(
        connection_,
        hostname,
        username,
        password,
        database,
        port,
        nullptr,
        0);
    if (nullptr == success) {
        MySqlException mse(connection_);
        mysql_close(connection_);
        throw mse;
    }
}


MySql::~MySql() {
    mysql_close(connection_);
}


my_ulonglong MySql::runCommand(const char* const command) {
    if (0 != mysql_real_query(connection_, command, strlen(command))) {
        throw MySqlException(connection_);
    }

    // If the user ran a SELECT statement or something else, at least warn them
    const my_ulonglong affectedRows = mysql_affected_rows(connection_);
    if ((my_ulonglong) - 1 == affectedRows) {
        // Clean up after the query
        MYSQL_RES* const result = mysql_store_result(connection_);
        mysql_free_result(result);

        throw MySqlException("Tried to run query with runCommand");
    }

    return affectedRows;
}

MySqlPreparedStatement MySql::prepareStatement(const char* const command) const {
    return MySqlPreparedStatement(command, connection_);
}

my_ulonglong MySql::inster_user_info(string databases, string table,  USER_Table data){
    my_ulonglong affectedRows = 0; 
    char cmd_change_databases[40];
    sprintf(cmd_change_databases,"USE %s",databases.c_str());
    runCommand(cmd_change_databases);
    char cmd_str[400];

    memset(cmd_str,0x00,sizeof(cmd_str));

    sprintf(cmd_str,"INSERT INTO %s (name,email, password,id_card,status,type,age)" 
                " VALUES ('%s', '%s', '%s', '%s', '%s', '%s', %d)",table.c_str(),get<1>(data).c_str(),get<2>(data).c_str(),get<3>(data).c_str(),
                get<4>(data).c_str(),get<5>(data).c_str(),get<6>(data).c_str(),get<7>(data));

    affectedRows = runCommand(cmd_str);

    return affectedRows;
}

my_ulonglong MySql::query_tabel_all(string databases, string table,char *cmd_str) {

    char cmd_change_databases[40];
    sprintf(cmd_change_databases,"USE %s",databases.c_str());
    runCommand(cmd_change_databases);
    // *****************************************
    // All commands use safe prepared statements
    // ***************************************** 
    sprintf(cmd_str,"SELECT * FROM %s",table.c_str());

    return 0;
}

my_ulonglong MySql::query_user_info(string databases, string table, string  key, string value, char *cmd_str) {

    char cmd_change_databases[40];
    sprintf(cmd_change_databases,"USE %s",databases.c_str());
    runCommand(cmd_change_databases);
    // *****************************************
    // All commands use safe prepared statements
    // ***************************************** 
    sprintf(cmd_str,"SELECT * FROM %s WHERE %s = '%s'",table.c_str(),key.c_str(),value.c_str());

    return 0;
}
    // //删除数据
    // conn.runCommand("DELETE FROM user_info WHERE name = \"user2\"");
    // //更新数据
    // conn.runCommand("UPDATE user_info  set age = 99,status = \"aaa\" WHERE name = \"user3\"");
    // //清空表
    // conn.runCommand("truncate table user_info");
    // //重置ID
    // conn.runCommand("alter table user_info auto_increment=1");
    // //跟新主键
    // conn.runCommand("alter table user_info drop id");//删除原主键
    // conn.runCommand("alter table user_info ADD id MEDIUMINT( 8 ) NOT NULL FIRST");//添加新主键
    // conn.runCommand("alter table user_info MODIFY COLUMN id MEDIUMINT( 8 ) NOT NULL AUTO_INCREMENT,ADD PRIMARY KEY(id)");//设置自增
    // //表table添加新属性
    // conn.runCommand("alter table user_info ADD test int(4) default 0");
    // //表table删除某一个属性
    // conn.runCommand("alter table user_info drop test");

my_ulonglong MySql::delete_user_info(string databases, string table, char* key, char *value){
    char cmd_change_databases[40];
    sprintf(cmd_change_databases,"USE %s",databases.c_str());
    runCommand(cmd_change_databases);
    char cmd_str[100];
    sprintf(cmd_str,"DELETE FROM %s WHERE %s = %s",table.c_str(),key,value);
    // //删除数据
    const my_ulonglong affectedRows = runCommand(cmd_str);
    ////跟新主键
    //删除原主键
    sprintf(cmd_str,"alter table %s drop id",table.c_str());
    runCommand(cmd_str);
    //添加新主键
    sprintf(cmd_str,"alter table %s ADD id MEDIUMINT( 8 ) NOT NULL FIRST",table.c_str());
    runCommand(cmd_str);
    //设置自增
    sprintf(cmd_str,"alter table %s MODIFY COLUMN id MEDIUMINT( 8 ) NOT NULL AUTO_INCREMENT,ADD PRIMARY KEY(id)",table.c_str());
    runCommand(cmd_str);
    return affectedRows;
}

my_ulonglong MySql::update_user_info(string databases, string table, char* key_id, char* keyid_value, char* key, char *value){
    char cmd_change_databases[40];
    sprintf(cmd_change_databases,"USE %s",databases.c_str());
    runCommand(cmd_change_databases);
    char cmd_str[100];
    sprintf(cmd_str,"UPDATE %s  set %s = \"%s\" WHERE %s = \"%s\"",table.c_str(),key,value,key_id,keyid_value);
    // //删除数据
    const my_ulonglong affectedRows = runCommand(cmd_str);
    return affectedRows;
}


