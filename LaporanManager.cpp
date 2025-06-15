#include "LaporanManager.h"
#include <fstream> // Untuk menulis file
#include <vector>
#include "Models.h"

LaporanManager::LaporanManager(Repository& repo) : m_repo(repo) {}

// Fungsi privat untuk mengubah data menjadi format string CSV
std::string LaporanManager::formatKeCSV(const std::vector<AbsensiRecord>& data) {
    std::string csv_content = "NPM,Nama Mahasiswa,Status Kehadiran\n"; // Header CSV
    for (const auto& record : data) {
        csv_content += record.npm + "," + record.nama + "," + record.status + "\n";
    }
    return csv_content;
}

bool LaporanManager::buatLaporanAbsensi(const std::string& nama_file, int kelas_id, const std::string& tanggal) {
    // 1. Ambil data dari repository
    std::vector<AbsensiRecord> data = m_repo.ambilDataKehadiran(kelas_id, tanggal);

    if (data.empty()) {
        return false; // Tidak ada data untuk dilaporkan
    }

    // 2. Format data ke CSV
    std::string csv_data = formatKeCSV(data);

    // 3. Simpan ke file
    std::ofstream file_output(nama_file);
    if (!file_output.is_open()) {
        return false; // Gagal membuka file
    }

    file_output << csv_data;
    file_output.close();

    return true;
}