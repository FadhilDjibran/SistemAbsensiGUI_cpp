#include "Repository.h"
#include <cstdio> // untuk sprintf

Repository::Repository(Database& database) : db(database) {}

void Repository::setupStrukturAwal() {
    MYSQL* conn = db.getConnection();
    if (!conn) return;
    
    mysql_query(conn, "CREATE TABLE IF NOT EXISTS mahasiswa (id INT AUTO_INCREMENT PRIMARY KEY, npm VARCHAR(20) UNIQUE NOT NULL, nama VARCHAR(255) NOT NULL)");
    mysql_query(conn, "CREATE TABLE IF NOT EXISTS kelas (id INT AUTO_INCREMENT PRIMARY KEY, kode_kelas VARCHAR(10) UNIQUE NOT NULL, nama_kelas VARCHAR(255) NOT NULL)");
    mysql_query(conn, "CREATE TABLE IF NOT EXISTS kehadiran (id INT AUTO_INCREMENT PRIMARY KEY, mahasiswa_id INT NOT NULL, kelas_id INT NOT NULL, tanggal DATE NOT NULL, status ENUM('Hadir','Izin','Sakit','Alpa') NOT NULL, UNIQUE KEY absensi_unik (mahasiswa_id, kelas_id, tanggal), FOREIGN KEY (mahasiswa_id) REFERENCES mahasiswa(id), FOREIGN KEY (kelas_id) REFERENCES kelas(id) ON DELETE CASCADE)");
    mysql_query(conn, "INSERT IGNORE INTO kelas (kode_kelas, nama_kelas) VALUES ('PBO', 'Pemrograman Berbasis Objek'), ('JARKOM', 'Jaringan Komputer'), ('DA', 'Desain Antarmuka'), ('RPL', 'Rekayasa Perangkat Lunak'), ('ADS', 'Analisis & Desain Sistem'), ('KCB', 'Kecerdasan Buatan'), ('PEMWEB', 'Pemrogramam Web')");
    mysql_query(conn, "INSERT IGNORE INTO mahasiswa (npm, nama) VALUES ('23081010030', 'Fadhil Djibran'), ('23081010001', 'Muhammad Syaifudin Afandi'), ('22081010279', 'Gilang Ramadhan S'), ('23081010999', 'Doni Firmansyah'), ('23081010998', 'Eka Putri')");
}

std::vector<Mahasiswa> Repository::ambilSemuaMahasiswa() {
    std::vector<Mahasiswa> hasil;
    MYSQL* conn = db.getConnection();
    if (!conn || mysql_query(conn, "SELECT id, npm, nama FROM mahasiswa ORDER BY npm ASC")) {
        return hasil;
    }
    
    MYSQL_RES* result = mysql_store_result(conn);
    if (result) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
            hasil.push_back({atoi(row[0]), row[1], row[2]});
        }
        mysql_free_result(result);
    }
    return hasil;
}

std::vector<Kelas> Repository::ambilSemuaKelas() {
    std::vector<Kelas> hasil;
    MYSQL* conn = db.getConnection();
    if (!conn || mysql_query(conn, "SELECT id, kode_kelas, nama_kelas FROM kelas ORDER BY nama_kelas ASC")) {
        return hasil;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
            hasil.push_back({atoi(row[0]), row[1], row[2]});
        }
        mysql_free_result(result);
    }
    return hasil;
}

bool Repository::simpanAbsensi(int kelas_id, const std::string& tanggal, const std::map<int, int>& statusKehadiran) {
    MYSQL* conn = db.getConnection();
    if (!conn) return false;

    const char* status_enum[] = {"Hadir", "Izin", "Sakit", "Alpa"};
    int total_sukses = 0;
    
    for (const auto& pair : statusKehadiran) {
        int mahasiswa_id = pair.first;
        int status_idx = pair.second;
        
        char query[512];
        sprintf(query, "INSERT INTO kehadiran (mahasiswa_id, kelas_id, tanggal, status) VALUES (%d, %d, '%s', '%s') ON DUPLICATE KEY UPDATE status = VALUES(status)",
                mahasiswa_id, kelas_id, tanggal.c_str(), status_enum[status_idx]);
        
        if (!mysql_query(conn, query)) {
            total_sukses++;
        }
    }
    return total_sukses == statusKehadiran.size();
}

std::vector<AbsensiRecord> Repository::ambilDataKehadiran(int kelas_id, const std::string& tanggal) {
    std::vector<AbsensiRecord> hasil;
    MYSQL* conn = db.getConnection();
    if (!conn || kelas_id == -1 || tanggal.empty()) {
        return hasil;
    }

    char query[512];
    // Ini adalah query JOIN untuk menggabungkan 2 tabel
    sprintf(query, "SELECT m.npm, m.nama, k.status "
                   "FROM kehadiran k "
                   "JOIN mahasiswa m ON k.mahasiswa_id = m.id "
                   "WHERE k.kelas_id = %d AND k.tanggal = '%s' "
                   "ORDER BY m.npm ASC",
                   kelas_id, tanggal.c_str());

    if (mysql_query(conn, query)) {
        // Handle error jika perlu, misalnya dengan logging
        return hasil;
    }

    MYSQL_RES* result = mysql_store_result(conn);
    if (result) {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
            hasil.push_back({row[0] ? row[0] : "", row[1] ? row[1] : "", row[2] ? row[2] : ""});
        }
        mysql_free_result(result);
    }
    return hasil;
}

bool Repository::tambahMahasiswa(const std::string& npm, const std::string& nama) {
    MYSQL* conn = db.getConnection();
    if (!conn) return false;

    // Escape string untuk keamanan dasar dari SQL Injection
    char escaped_npm[256];
    char escaped_nama[512];
    mysql_real_escape_string(conn, escaped_npm, npm.c_str(), npm.length());
    mysql_real_escape_string(conn, escaped_nama, nama.c_str(), nama.length());

    char query[1024];
    sprintf(query, "INSERT INTO mahasiswa (npm, nama) VALUES ('%s', '%s')", escaped_npm, escaped_nama);

    if (mysql_query(conn, query)) {
        // Gagal (kemungkinan karena NPM sudah ada)
        return false;
    }
    return true;
}