#include "Database.h"
#include <windows.h> // Untuk MessageBox

Database::Database() : conn(nullptr) {}

Database::~Database() {
    disconnect();
}

bool Database::connect(const char* host, const char* user, const char* pass, const char* db_name, unsigned int port) {
    conn = mysql_init(NULL);
    if (conn == NULL) {
        MessageBoxA(NULL, "mysql_init() gagal.", "Error Database", MB_OK | MB_ICONERROR);
        return false;
    }
    if (mysql_real_connect(conn, host, user, pass, db_name, port, NULL, 0) == NULL) {
        // Coba buat database jika koneksi awal gagal karena DB tidak ada
        conn = mysql_init(NULL);
        mysql_real_connect(conn, host, user, pass, NULL, port, NULL, 0);
        if(mysql_query(conn, "CREATE DATABASE IF NOT EXISTS db_absensi")) {
             MessageBoxA(NULL, mysql_error(conn), "Koneksi & Buat DB Gagal", MB_OK | MB_ICONERROR);
             return false;
        }
        mysql_select_db(conn, db_name);
    }
    return true;
}

void Database::disconnect() {
    if (conn != nullptr) {
        mysql_close(conn);
        conn = nullptr;
    }
}

MYSQL* Database::getConnection() {
    return conn;
}