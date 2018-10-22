#ifndef MYSQL_HPP_
#define MYSQL_HPP_

#include <cassert>
#include <cstdint>
#include <cstring>
#include <mysql/mysql.h>

#include <boost/lexical_cast.hpp>
#include <string>
#include <tuple>
#include <typeinfo>
#include <utility>
#include <vector>
#include <sstream>
#include <iostream>
#include <memory>
#include <tuple>
#include <string>
#include "InputBinder.hpp"
#include "MySqlException.hpp"
#include "MySqlPreparedStatement.hpp"
#include "OutputBinder.hpp"

using std::stringstream;
using std::basic_ostream;
using std::cin;
using std::cout;
using std::endl;
using std::get;
using std::ostream;
using std::string;
using std::tuple;
using std::unique_ptr;
using std::vector;

#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 6)
#ifndef nullptr
// I know that this isn't a perfect substitution, but I'm lazy
#define nullptr 0
#endif
#endif

template <std::size_t> struct int_ {};  // compile-time counter
template <typename Char, typename Traits, typename... Args>
ostream& operator<<(basic_ostream<Char, Traits>& out, tuple<Args...> const& t);

template <typename T>
ostream& operator<<(ostream& out, const unique_ptr<T>& ptr);

template <typename Char, typename Traits, typename Tuple, std::size_t I>
void printTuple(basic_ostream<Char, Traits>& out, Tuple const& t, int_<I>);

template <typename Char, typename Traits, typename Tuple>
void printTuple(basic_ostream<Char, Traits>& out, Tuple const& t, int_<0>);

template <typename Tuple>
void printTupleVector(const vector<Tuple>& v);
        
template<typename Char, typename Traits, typename Tuple, size_t I>
void printTuple(basic_ostream<Char, Traits>& out, Tuple const& t, int_<I>) {
    printTuple(out, t, int_<I - 1>());
    out << ", " << get<I>(t);
}

template<typename Char, typename Traits, typename Tuple>
void printTuple(basic_ostream<Char, Traits>& out, Tuple const& t, int_<0>) {
      out << get<0>(t);
}

template <typename Tuple>
void printTupleVector(const vector<Tuple>& v) {
#if __GNUC__ >= 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
    for (const auto& item : v)
    {
        cout << item << endl;
    }
#elif __GNUC__ >= 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4)
    auto end = v.cend();
    for (auto item(v.cbegin()); item != end; ++item) {
        cout << *item<< endl;
    }
#else
    vector<Tuple>::const_iterator end(users.end());
    for (
        vector<Tuple>::const_iterator item(v.begin());
        item != end;
        ++item
    ) {
        cout << *item << endl;
    }
#endif
}

template<typename Char, typename Traits, typename... Args>
ostream& operator<<(
    basic_ostream<Char, Traits>& out,
    tuple<Args...> const& t
) {
    out << "(";
    printTuple(out, t, int_<sizeof...(Args) - 1>());
    out << ")";
    return out;
}

template <typename T>
ostream& operator<<(ostream& out, const unique_ptr<T>& ptr) {
    if (nullptr != ptr.get()) {
        out << *ptr;
    } else {
        out << "NULL";
    }
    return out;
}

typedef tuple<
    int,
    string,
    string,
    string,
    string,
    string,
    string,
    int
>USER_Table;

typedef tuple<
    int,
    string,
    string,
    string,
    string,
    string,
    double,
    double,
    double,
    double,
    double,
    float,
    double,
    double,
    double
>CAR_Table;

class MySql {
    public:
        MySql(
            const char* const hostname,
            const char* const username,
            const char* const password,
            const char* const database,
            const uint16_t port = 3306);

        MySql(
            const char* hostname,
            const char* username,
            const char* password,
            const uint16_t port = 3306);

        ~MySql();

        MySql(const MySql& rhs) = delete;
        MySql(MySql&& rhs) = delete;
        MySql& operator=(const MySql& rhs) = delete;
        MySql& operator=(MySql&& rhs) = delete;

        /**
         * Normal query. Results are stored in the given vector.
         * @param query The query to run.
         * @param results A vector of tuples to store the results in.
         * @param args Arguments to bind to the query.
         */
        template <typename... InputArgs, typename... OutputArgs>
        void runQuery(
            std::vector<std::tuple<OutputArgs...>>* const results,
            const char* const query,
            // Args needs to be sent by reference, because the values need to be
            // nontemporary (that is, lvalues) so that their memory locations
            // can be bound to MySQL's prepared statement API
            const InputArgs&... args) const;

        /**
         * Command that doesn't return results, like "USE yelp" or
         * "INSERT INTO user VALUES ('Brandon', 28)".
         * @param query The query to run.
         * @param args Arguments to bind to the query.
         * @return The number of affected rows.
         */
        /// @{
        template <typename... Args>
        my_ulonglong runCommand(
            const char* const command,
            // Args needs to be sent by reference, because the values need to be
            // nontemporary (that is, lvalues) so that their memory locations
            // can be bound to MySQL's prepared statement API
            const Args&... args);
        my_ulonglong runCommand(const char* const command);
        /// @}

        /**
         * Prepare a statement for multiple executions with different bound
         * parameters. If you're running a one off query or statement, you
         * should use runQuery or runCommand instead.
         * @param query The query to prepare.
         * @return A prepared statement object.
         */
        MySqlPreparedStatement prepareStatement(const char* statement) const;

        /**
         * Run the command version of a prepared statement.
         */
        /// @{
        template <typename... Args>
        my_ulonglong runCommand(
            const MySqlPreparedStatement& statement,
            const Args&... args);
        my_ulonglong runCommand(const MySqlPreparedStatement& statement);
        /// @}

        /**
         * Run the query version of a prepared statement.
         */
        template <typename... InputArgs, typename... OutputArgs>
        void runQuery(
            std::vector<std::tuple<OutputArgs...>>* results,
            const MySqlPreparedStatement& statement,
            const InputArgs&...) const;
        /*
        * and function to optera table
         */
        my_ulonglong query_tabel_all(string databases, string table,char *cmd_str);
        my_ulonglong inster_user_info(string databases, string table, USER_Table data);
        my_ulonglong inster_car_info(string databases, string table,  CAR_Table data);
        my_ulonglong query_user_info(string databases, string table, string  key, string value,char *cmd_str);
        my_ulonglong delete_user_info(string databases, string table, char* key, char *value);
        my_ulonglong update_user_info(string databases, string table, char* key_id, char* keyid_value, char* key, char *value);
        my_ulonglong update_pos(string databases, string table, char* keyid_value, double latitude,double longitude,double x, double y, double z, float speed, double yaw, double pitch, double roll);

    private:
        MYSQL* connection_;
};

template <typename... Args>
my_ulonglong MySql::runCommand(
    const char* const command,
    const Args&... args
) {
    MySqlPreparedStatement statement(prepareStatement(command));
    return runCommand(statement, args...);
}


template <typename... Args>
my_ulonglong MySql::runCommand(
    const MySqlPreparedStatement& statement,
    const Args&... args
) {
    // Commands (e.g. INSERTs or DELETEs) should always have this set to 0
    if (0 != statement.getFieldCount()) {
        throw MySqlException("Tried to run query with runCommand");
    }

    if (sizeof...(args) != statement.getParameterCount()) {
        std::string errorMessage;
        errorMessage += "Incorrect number of parameters; command required ";
        errorMessage += boost::lexical_cast<std::string>(
            statement.getParameterCount());
        errorMessage += " but ";
        errorMessage += boost::lexical_cast<std::string>(sizeof...(args));
        errorMessage += " parameters were provided.";
        throw MySqlException(errorMessage);
    }

    std::vector<MYSQL_BIND> bindParameters;
    bindParameters.resize(statement.getParameterCount());
    bindInputs<Args...>(&bindParameters, args...);
    if (0 != mysql_stmt_bind_param(
        statement.statementHandle_,
        bindParameters.data())
    ) {
        throw MySqlException(statement);
    }

    if (0 != mysql_stmt_execute(statement.statementHandle_)) {
        throw MySqlException(statement);
    }

    // If the user ran a SELECT statement or something else, at least warn them
    const auto affectedRows = mysql_stmt_affected_rows(
        statement.statementHandle_);
    if ((static_cast<decltype(affectedRows)>(-1)) == affectedRows) {
        throw MySqlException("Tried to run query with runCommand");
    }

    return affectedRows;
}


template <typename... InputArgs, typename... OutputArgs>
void MySql::runQuery(
    std::vector<std::tuple<OutputArgs...>>* const results,
    const char* const query,
    const InputArgs&... args
) const {
    assert(nullptr != results);
    assert(nullptr != query);
    MySqlPreparedStatement statement(prepareStatement(query));
    runQuery(results, statement, args...);
}


template <typename... InputArgs, typename... OutputArgs>
void MySql::runQuery(
    std::vector<std::tuple<OutputArgs...>>* const results,
    const MySqlPreparedStatement& statement,
    const InputArgs&... args
) const {
    assert(nullptr != results);

    // SELECTs should always return something. Commands (e.g. INSERTs or
    // DELETEs) should always have this set to 0.
    if (0 == statement.getFieldCount()) {
        throw MySqlException("Tried to run command with runQuery");
    }

    // Bind the input parameters
    // Check that the parameter count is right
    if (sizeof...(InputArgs) != statement.getParameterCount()) {
        std::string errorMessage;

        errorMessage += "Incorrect number of input parameters; query required ";
        errorMessage += boost::lexical_cast<std::string>(
            statement.getParameterCount());
        errorMessage += " but ";
        errorMessage += boost::lexical_cast<std::string>(sizeof...(args));
        errorMessage += " parameters were provided.";
        throw MySqlException(errorMessage);
    }

    std::vector<MYSQL_BIND> inputBindParameters;
    inputBindParameters.resize(statement.getParameterCount());
    bindInputs<InputArgs...>(&inputBindParameters, args...);
    if (0 != mysql_stmt_bind_param(
        statement.statementHandle_,
        inputBindParameters.data())
    ) {
        throw MySqlException(statement);
    }

    setResults<OutputArgs...>(statement, results);
}


#endif  // MYSQL_HPP_
