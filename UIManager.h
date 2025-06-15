#pragma once
#include "Models.h"
#include <map>

class UIManager {
private:
    // State untuk UI
    int kelas_terpilih_idx;
    char tanggal_buffer[11];
    std::string status_simpan;
    bool tanggal_diinisialisasi;
    char m_npm_buffer[128];
    char m_nama_buffer[128];
    std::string m_status_tambah_mhs;
    char m_laporan_tanggal_buffer[11];
    int m_laporan_kelas_idx;
    std::string m_status_laporan;
    
public:
    UIManager();
    void render(const std::vector<Kelas>& daftar_kelas, 
                const std::vector<Mahasiswa>& daftar_mahasiswa, 
                std::map<int, int>& status_kehadiran, 
                bool& tombol_simpan_diklik, 
                const std::vector<AbsensiRecord>& data_kehadiran);
    int getKelasIdTerpilih(const std::vector<Kelas>& daftar_kelas);
    std::string getTanggalTerpilih();
    void setStatusSimpan(const std::string& status);
    void renderManajemenMahasiswa(bool& tombol_tambah_diklik);
    std::string getNpmInput();
    std::string getNamaInput();
    void setStatusTambahMhs(const std::string& status);
    void renderLaporan(const std::vector<Kelas>& daftar_kelas, bool& tombol_buat_laporan_diklik);
    int getLaporanKelasId(const std::vector<Kelas>& daftar_kelas);
    std::string getLaporanTanggal();
    void setStatusLaporan(const std::string& status);
    int getLaporanKelasIndex() const;
};