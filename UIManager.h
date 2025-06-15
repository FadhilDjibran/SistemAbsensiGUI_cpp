#pragma once
#include "Models.h"
#include <map>
#include <string> 
#include <vector> 

class UIManager {
private:
    // State untuk UI Absensi
    int m_kelas_terpilih_idx;
    char m_tanggal_buffer[11];
    std::string m_status_simpan;

    // State untuk UI Manajemen Mahasiswa
    char m_npm_buffer[128];
    char m_nama_buffer[128];
    std::string m_status_tambah_mhs;

    // State untuk UI Laporan
    char m_laporan_tanggal_buffer[11];
    int m_laporan_kelas_idx;
    std::string m_status_laporan;

public:
    UIManager();
    
    // Fungsi render utama
    void render(const std::vector<Kelas>& daftar_kelas, const std::vector<Mahasiswa>& daftar_mahasiswa, std::map<int, int>& status_kehadiran, bool& tombol_simpan_diklik, const std::vector<AbsensiRecord>& data_kehadiran);
    
    // Fungsi render untuk Manajemen Mahasiswa
    void renderManajemenMahasiswa(const std::vector<Mahasiswa>& daftar_mahasiswa, bool& tombol_tambah_diklik, int& id_mahasiswa_edit, int& id_mahasiswa_hapus);
    
    // Fungsi render untuk popup
    void renderPopupEditMahasiswa(bool& show_popup, Mahasiswa& mhs, bool& tombol_simpan_perubahan_diklik);
    void renderPopupHapusMahasiswa(bool& show_popup, const Mahasiswa& mhs, bool& tombol_konfirmasi_hapus_diklik);

    // Fungsi render untuk Laporan
    void renderLaporan(const std::vector<Kelas>& daftar_kelas, bool& tombol_buat_laporan_diklik);

    // Getter & Setters
    int getKelasIdTerpilih(const std::vector<Kelas>& daftar_kelas);
    std::string getTanggalTerpilih();
    void setStatusSimpan(const std::string& status);
    
    std::string getNpmInput();
    std::string getNamaInput();
    void setStatusTambahMhs(const std::string& status);
    
    int getLaporanKelasIndex() const;
    int getLaporanKelasId(const std::vector<Kelas>& daftar_kelas);
    std::string getLaporanTanggal();
    void setStatusLaporan(const std::string& status);
};