#include "UIManager.h"
#include "DateUtil.h"
#include "imgui/imgui.h"
#include "Models.h" 
using namespace std;
using namespace ImGui;

// Konstruktor
UIManager::UIManager() {
    // State untuk Tab Absensi
    m_kelas_terpilih_idx = 0;
    strncpy(m_tanggal_buffer, DateUtil::getCurrentDate().c_str(), 10);
    m_tanggal_buffer[10] = '\0';
    m_status_simpan = "";

    // State untuk Form Tambah Mahasiswa
    m_npm_buffer[0] = '\0';
    m_nama_buffer[0] = '\0';
    m_status_tambah_mhs = "";

    // State untuk Form Laporan
    strncpy(m_laporan_tanggal_buffer, DateUtil::getCurrentDate().c_str(), 10);
    m_laporan_tanggal_buffer[10] = '\0';
    m_laporan_kelas_idx = 0;
    m_status_laporan = "Pilih kelas dan tanggal untuk membuat laporan.";
}

// Fungsi render Tab Absensi 
void UIManager::render(const vector<Kelas>& daftar_kelas, const vector<Mahasiswa>& daftar_mahasiswa, map<int, int>& status_kehadiran, bool& tombol_simpan_diklik, const vector<AbsensiRecord>& data_kehadiran) {
    tombol_simpan_diklik = false;
    Begin("Sistem Absensi Mahasiswa");

    Text("INPUT ABSENSI");
    Separator();
    
    if (!daftar_kelas.empty() && m_kelas_terpilih_idx < daftar_kelas.size()) {
        if (BeginCombo("Pilih Kelas", daftar_kelas[m_kelas_terpilih_idx].nama_kelas.c_str())) {
            for (int n = 0; n < daftar_kelas.size(); n++) {
                const bool is_selected = (m_kelas_terpilih_idx == n);
                if (Selectable(daftar_kelas[n].nama_kelas.c_str(), is_selected))
                    m_kelas_terpilih_idx = n;
                if (is_selected) SetItemDefaultFocus();
            }
            EndCombo();
        }
    }
    InputText("Tanggal (YYYY-MM-DD)", m_tanggal_buffer, IM_ARRAYSIZE(m_tanggal_buffer));
    
    if (BeginTable("tabel_input_absensi", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        TableSetupColumn("NPM"); TableSetupColumn("Nama Mahasiswa"); TableSetupColumn("Status Kehadiran");
        TableHeadersRow();
        for (const auto& mhs : daftar_mahasiswa) {
            TableNextRow();
            TableSetColumnIndex(0); Text("%s", mhs.npm.c_str());
            TableSetColumnIndex(1); Text("%s", mhs.nama.c_str());
            TableSetColumnIndex(2);
            PushID(mhs.id);
            RadioButton("Hadir", &status_kehadiran[mhs.id], 0); SameLine();
            RadioButton("Izin", &status_kehadiran[mhs.id], 1); SameLine();
            RadioButton("Sakit", &status_kehadiran[mhs.id], 2); SameLine();
            RadioButton("Alpha", &status_kehadiran[mhs.id], 3);
            PopID();
        }
        EndTable();
    }
    
    if (Button("Simpan Absensi", ImVec2(150, 30))) { tombol_simpan_diklik = true; }
    if(!m_status_simpan.empty()) { Text("Status Simpan: %s", m_status_simpan.c_str()); }
    
    Dummy(ImVec2(0.0f, 20.0f));
    Text("DATA ABSENSI TERSIMPAN");
    Separator();
    
    if (BeginTable("tabel_hasil", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        TableSetupColumn("NPM"); TableSetupColumn("Nama Mahasiswa"); TableSetupColumn("Status");
        TableHeadersRow();
        for (const auto& absen : data_kehadiran) {
            TableNextRow();
            TableSetColumnIndex(0); Text("%s", absen.npm.c_str());
            TableSetColumnIndex(1); Text("%s", absen.nama.c_str());
            TableSetColumnIndex(2); Text("%s", absen.status.c_str());
        }
        EndTable();
    }
    End();
}

// Fungsi render Tab Manajemen Mahasiswa
void UIManager::renderManajemenMahasiswa(const vector<Mahasiswa>& daftar_mahasiswa, bool& tombol_tambah_diklik, int& id_mahasiswa_edit, int& id_mahasiswa_hapus) {

    tombol_tambah_diklik = false;
    id_mahasiswa_edit = -1;
    id_mahasiswa_hapus = -1;

    Begin("Manajemen Mahasiswa");
    
    // Form Menambah Mahasiswa Baru
    Text("Tambah Mahasiswa Baru");
    Separator();
    InputText("NPM", m_npm_buffer, IM_ARRAYSIZE(m_npm_buffer));
    InputText("Nama Lengkap", m_nama_buffer, IM_ARRAYSIZE(m_nama_buffer));
    if (Button("Tambah Mahasiswa")) {
        tombol_tambah_diklik = true;
    }
    Text("Status: %s", m_status_tambah_mhs.c_str());
    
    Dummy(ImVec2(0.0f, 20.0f));
    
    // Tabel Daftar Mahasiswa
    Text("Daftar Mahasiswa Terdaftar");
    Separator();
    if (BeginTable("daftar_mhs", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        TableSetupColumn("ID");
        TableSetupColumn("NPM");
        TableSetupColumn("Nama");
        TableSetupColumn("Aksi");
        TableHeadersRow();

        for (const auto& mhs : daftar_mahasiswa) {
            TableNextRow();
            TableSetColumnIndex(0); Text("%d", mhs.id);
            TableSetColumnIndex(1); Text("%s", mhs.npm.c_str());
            TableSetColumnIndex(2); Text("%s", mhs.nama.c_str());
            TableSetColumnIndex(3);
            PushID(mhs.id); // ID unik 
            if (Button("Ubah")) { id_mahasiswa_edit = mhs.id; } // ID yang diubah
            SameLine();
            if (Button("Hapus")) { id_mahasiswa_hapus = mhs.id; } // ID yang diubah
            PopID();
        }
        EndTable();
    }
    End();
}

// Popup edit
void UIManager::renderPopupEditMahasiswa(bool& show_popup, Mahasiswa& mhs, bool& tombol_simpan_perubahan_diklik) {
    tombol_simpan_perubahan_diklik = false;
    if (show_popup) {
        OpenPopup("Ubah Data Mahasiswa");
    }
    if (BeginPopupModal("Ubah Data Mahasiswa", &show_popup, ImGuiWindowFlags_AlwaysAutoResize)) {
        static char npm_edit_buffer[128];
        static char nama_edit_buffer[128];
        
        if (IsWindowAppearing()) {
            strncpy(npm_edit_buffer, mhs.npm.c_str(), sizeof(npm_edit_buffer) - 1);
            strncpy(nama_edit_buffer, mhs.nama.c_str(), sizeof(nama_edit_buffer) - 1);
        }

        InputText("NPM", npm_edit_buffer, sizeof(npm_edit_buffer));
        InputText("Nama", nama_edit_buffer, sizeof(nama_edit_buffer));
        Separator();
        
        if (Button("Simpan Perubahan", ImVec2(120, 0))) {
            mhs.npm = npm_edit_buffer;
            mhs.nama = nama_edit_buffer;
            tombol_simpan_perubahan_diklik = true;
            CloseCurrentPopup();
            show_popup = false;
        }
        SameLine();
        if (Button("Batal", ImVec2(120, 0))) {
            CloseCurrentPopup();
            show_popup = false;
        }
        EndPopup();
    }
}

// Popup hapus
void UIManager::renderPopupHapusMahasiswa(bool& show_popup, const Mahasiswa& mhs, bool& tombol_konfirmasi_hapus_diklik) {
    tombol_konfirmasi_hapus_diklik = false;
    if (show_popup) {
        OpenPopup("Konfirmasi Hapus");
    }
    if (BeginPopupModal("Konfirmasi Hapus", &show_popup, ImGuiWindowFlags_AlwaysAutoResize)) {
        Text("Anda yakin ingin menghapus mahasiswa ini?");
        Text("NPM: %s", mhs.npm.c_str());
        Text("Nama: %s", mhs.nama.c_str());
        Text("\nSemua data kehadiran yang terkait akan ikut terhapus.\nAksi ini tidak bisa dibatalkan.\n");
        Separator();
        
        if (Button("Ya, Hapus", ImVec2(120, 0))) {
            tombol_konfirmasi_hapus_diklik = true;
            CloseCurrentPopup();
            show_popup = false;
        }
        SetItemDefaultFocus();
        SameLine();
        if (Button("Batal", ImVec2(120, 0))) {
            CloseCurrentPopup();
            show_popup = false;
        }
        EndPopup();
    }
}

// Render Laporan 
void UIManager::renderLaporan(const vector<Kelas>& daftar_kelas, bool& tombol_buat_laporan_diklik) {
    tombol_buat_laporan_diklik = false;
    Begin("Laporan Absensi");
    Text("Buat Laporan Absensi per Kelas (format .csv)");
    Separator();
    if (!daftar_kelas.empty() && m_laporan_kelas_idx < daftar_kelas.size()) {
        if (BeginCombo("Pilih Kelas Laporan", daftar_kelas[m_laporan_kelas_idx].nama_kelas.c_str())) {
            for (int n = 0; n < daftar_kelas.size(); n++) {
                if (Selectable(daftar_kelas[n].nama_kelas.c_str(), (m_laporan_kelas_idx == n)))
                    m_laporan_kelas_idx = n;
            }
            EndCombo();
        }
    }
    InputText("Tanggal Laporan", m_laporan_tanggal_buffer, IM_ARRAYSIZE(m_laporan_tanggal_buffer));
    if (Button("Buat Laporan")) {
        tombol_buat_laporan_diklik = true;
    }
    Text("Status: %s", m_status_laporan.c_str());
    End();
}

//Getter dan Setter
int UIManager::getKelasIdTerpilih(const vector<Kelas>& df) { if (df.empty() || m_kelas_terpilih_idx >= df.size()) return -1; return df[m_kelas_terpilih_idx].id; }
string UIManager::getTanggalTerpilih() { return string(m_tanggal_buffer); }
void UIManager::setStatusSimpan(const string& s) { m_status_simpan = s; }
string UIManager::getNpmInput() { return string(m_npm_buffer); }
string UIManager::getNamaInput() { return string(m_nama_buffer); }
void UIManager::setStatusTambahMhs(const string& s) { 
    m_status_tambah_mhs = s; 
    if(s.find("Sukses") != string::npos) { m_npm_buffer[0] = '\0'; m_nama_buffer[0] = '\0'; }
}
int UIManager::getLaporanKelasIndex() const { return m_laporan_kelas_idx; }
int UIManager::getLaporanKelasId(const vector<Kelas>& df) { if (df.empty() || m_laporan_kelas_idx >= df.size()) return -1; return df[m_laporan_kelas_idx].id; }
string UIManager::getLaporanTanggal() { return string(m_laporan_tanggal_buffer); }
void UIManager::setStatusLaporan(const string& s) { m_status_laporan = s; }