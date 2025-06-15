#include "LaporanManager.h"
#include <fstream> // Untuk menulis file
#include <vector>
#include "Models.h"
using namespace std;

LaporanManager::LaporanManager(Repository& repo) : m_repo(repo) {}

// Mengubah data menjadi format string CSV
string LaporanManager::formatKeCSV(const vector<AbsensiRecord>& data) {
    string csv_content = "NPM,Nama Mahasiswa,Status Kehadiran\n"; // Header CSV
    for (const auto& record : data) {
        csv_content += record.npm + "," + record.nama + "," + record.status + "\n";
    }
    return csv_content;
}

bool LaporanManager::buatLaporanAbsensi(const string& nama_file, int kelas_id, const string& tanggal) {
    // Ambil data dari repository
    vector<AbsensiRecord> data = m_repo.ambilDataKehadiran(kelas_id, tanggal);

    if (data.empty()) {
        return false; // Tidak ada data
    }

    // Format data ke CSV
    string csv_data = formatKeCSV(data);

    // Simpan ke file
    ofstream file_output(nama_file);
    if (!file_output.is_open()) {
        return false; 
    }

    file_output << csv_data;
    file_output.close();

    return true;
}