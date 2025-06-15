#pragma once
#include <mysql.h>
#include <string>

class Database {
private:
    MYSQL* conn;

public:
    Database();
    ~Database();
    bool connect(const char* host, const char* user, const char* pass, const char* db_name, unsigned int port);
    void disconnect();
    MYSQL* getConnection();
};