#pragma once
#include "Database.h"
#include "Models.h"
#include <vector>
#include <map>

class Repository {
private:
    Database& db;

public:
    Repository(Database& database);
    void setupStrukturAwal();
    std::vector<Mahasiswa> ambilSemuaMahasiswa();
    std::vector<Kelas> ambilSemuaKelas();
    bool simpanAbsensi(int kelas_id, const std::string& tanggal, const std::map<int, int>& statusKehadiran);
    std::vector<AbsensiRecord> ambilDataKehadiran(int kelas_id, const std::string& tanggal);
    bool tambahMahasiswa(const std::string& npm, const std::string& nama);
    bool ubahMahasiswa(int id, const std::string& npm, const std::string& nama);
    bool hapusMahasiswa(int id);
};