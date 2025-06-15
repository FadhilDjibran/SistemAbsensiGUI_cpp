#pragma once
#include "Database.h"
#include "Repository.h"
#include "UIManager.h"
#include "Models.h"
#include "MasterDataManager.h"
#include "LaporanManager.h"

class Aplikasi {
private:
    Database db;
    Repository repo;
    UIManager ui;
    MasterDataManager masterDataManager;
    LaporanManager laporanManager;
    
    // Data state
    std::vector<Mahasiswa> daftar_mahasiswa;
    std::vector<Kelas> daftar_kelas;
    std::map<int, int> status_kehadiran;
    std::vector<AbsensiRecord> data_kehadiran_tersimpan;

    // State untuk Popups
    bool m_show_popup_edit;
    bool m_show_popup_hapus;
    Mahasiswa m_mhs_dipilih; // Dipilih untuk diubah/hapus

    void init_gui();
    void shutdown_gui();
    void main_loop();
    void load_initial_data();
    void muatDataKehadiranTersimpan();
    void simpan_data_absensi();
    void tambahMahasiswaBaru();
    void ubahMahasiswaTerpilih();
    void hapusMahasiswaTerpilih();
    void buatLaporanBaru();

public:
    Aplikasi();
    void run();
};