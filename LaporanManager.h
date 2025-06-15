#pragma once
#include "Repository.h"
#include <string>

class LaporanManager {
private:
    Repository& m_repo;
    std::string formatKeCSV(const std::vector<AbsensiRecord>& data);

public:
    LaporanManager(Repository& repo);
    bool buatLaporanAbsensi(const std::string& nama_file, int kelas_id, const std::string& tanggal);
};