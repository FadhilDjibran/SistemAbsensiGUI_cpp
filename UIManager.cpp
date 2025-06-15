#include "UIManager.h"
#include "DateUtil.h"
#include "imgui/imgui.h" // ImGui header

UIManager::UIManager() : kelas_terpilih_idx(0), tanggal_diinisialisasi(false), status_simpan("") {
    // Inisialisasi buffer tanggal saat objek dibuat
    strncpy(tanggal_buffer, DateUtil::getCurrentDate().c_str(), 10);
    tanggal_buffer[10] = '\0';
    m_npm_buffer[0] = '\0';
    m_nama_buffer[0] = '\0';
    m_status_tambah_mhs = "Silakan masukkan data mahasiswa baru.";
    strncpy(m_laporan_tanggal_buffer, DateUtil::getCurrentDate().c_str(), 10);
    m_laporan_tanggal_buffer[10] = '\0';
    m_laporan_kelas_idx = 0;
    m_status_laporan = "Pilih kelas dan tanggal untuk membuat laporan.";
}

void UIManager::render(const std::vector<Kelas>& daftar_kelas, const std::vector<Mahasiswa>& daftar_mahasiswa, std::map<int, int>& status_kehadiran, bool& tombol_simpan_diklik, const std::vector<AbsensiRecord>& data_kehadiran) {
    tombol_simpan_diklik = false;

    ImGui::SetNextWindowSize(ImVec2(680, 500), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiCond_FirstUseEver);
    ImGui::Begin("Sistem Absensi Mahasiswa");

    // --- Bagian Atas: Form Input dan Tabel Input Absensi ---
    ImGui::Text("INPUT ABSENSI");
    ImGui::Separator();
    // ... (kode combo box kelas dan input tanggal tetap sama) ...
    if (!daftar_kelas.empty()) {
        if (ImGui::BeginCombo("Pilih Kelas", daftar_kelas[kelas_terpilih_idx].nama_kelas.c_str())) {
            for (int n = 0; n < daftar_kelas.size(); n++) {
                const bool is_selected = (kelas_terpilih_idx == n);
                if (ImGui::Selectable(daftar_kelas[n].nama_kelas.c_str(), is_selected))
                    kelas_terpilih_idx = n;
                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
    ImGui::InputText("Tanggal (YYYY-MM-DD)", tanggal_buffer, IM_ARRAYSIZE(tanggal_buffer));
    
    // ... (kode tabel input absensi dengan radio button tetap sama) ...
    if (ImGui::BeginTable("tabel_input_absensi", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("NPM"); ImGui::TableSetupColumn("Nama Mahasiswa"); ImGui::TableSetupColumn("Status Kehadiran");
        ImGui::TableHeadersRow();
        for (const auto& mhs : daftar_mahasiswa) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("%s", mhs.npm.c_str());
            ImGui::TableSetColumnIndex(1); ImGui::Text("%s", mhs.nama.c_str());
            ImGui::TableSetColumnIndex(2);
            ImGui::PushID(mhs.id);
            ImGui::RadioButton("Hadir", &status_kehadiran[mhs.id], 0); ImGui::SameLine();
            ImGui::RadioButton("Izin", &status_kehadiran[mhs.id], 1); ImGui::SameLine();
            ImGui::RadioButton("Sakit", &status_kehadiran[mhs.id], 2); ImGui::SameLine();
            ImGui::RadioButton("Alpa", &status_kehadiran[mhs.id], 3);
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    
    if (ImGui::Button("Simpan Absensi", ImVec2(150, 30))) {
        tombol_simpan_diklik = true;
    }
    if(!status_simpan.empty()) { ImGui::Text("%s", status_simpan.c_str()); }
    
    ImGui::Dummy(ImVec2(0.0f, 20.0f)); // Spasi vertikal

    // --- Bagian Bawah: Tabel Data Absensi yang Sudah Tersimpan ---
    ImGui::Text("DATA ABSENSI TERSIMPAN");
    ImGui::Separator();
    
    if (ImGui::BeginTable("tabel_hasil", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
        ImGui::TableSetupColumn("NPM");
        ImGui::TableSetupColumn("Nama Mahasiswa");
        ImGui::TableSetupColumn("Status");
        ImGui::TableHeadersRow();

        for (const auto& absen : data_kehadiran) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("%s", absen.npm.c_str());
            ImGui::TableSetColumnIndex(1); ImGui::Text("%s", absen.nama.c_str());
            ImGui::TableSetColumnIndex(2); ImGui::Text("%s", absen.status.c_str());
        }
        ImGui::EndTable();
    }

    ImGui::End();
}

int UIManager::getKelasIdTerpilih(const std::vector<Kelas>& daftar_kelas) {
    if (daftar_kelas.empty()) return -1;
    return daftar_kelas[kelas_terpilih_idx].id;
}

std::string UIManager::getTanggalTerpilih() {
    return std::string(tanggal_buffer);
}

void UIManager::setStatusSimpan(const std::string& status) {
    status_simpan = status;
}

void UIManager::renderManajemenMahasiswa(bool& tombol_tambah_diklik) {
    tombol_tambah_diklik = false;
    ImGui::Begin("Manajemen Mahasiswa");
    
    ImGui::Text("Tambah Mahasiswa Baru");
    ImGui::Separator();
    
    ImGui::InputText("NPM", m_npm_buffer, IM_ARRAYSIZE(m_npm_buffer));
    ImGui::InputText("Nama Lengkap", m_nama_buffer, IM_ARRAYSIZE(m_nama_buffer));
    
    if (ImGui::Button("Tambah Mahasiswa")) {
        tombol_tambah_diklik = true;
    }
    
    ImGui::Text("Status: %s", m_status_tambah_mhs.c_str());
    
    ImGui::End();
}

std::string UIManager::getNpmInput() { return std::string(m_npm_buffer); }
std::string UIManager::getNamaInput() { return std::string(m_nama_buffer); }

void UIManager::setStatusTambahMhs(const std::string& status) {
    m_status_tambah_mhs = status;
    // Jika sukses, kosongkan buffer
    if (status.find("Sukses") != std::string::npos) {
        m_npm_buffer[0] = '\0';
        m_nama_buffer[0] = '\0';
    }
}

void UIManager::renderLaporan(const std::vector<Kelas>& daftar_kelas, bool& tombol_buat_laporan_diklik) {
    tombol_buat_laporan_diklik = false;
    ImGui::Begin("Laporan Absensi");
    
    ImGui::Text("Buat Laporan Absensi per Kelas (format .csv)");
    ImGui::Separator();
    
    if (!daftar_kelas.empty()) {
        if (ImGui::BeginCombo("Pilih Kelas Laporan", daftar_kelas[m_laporan_kelas_idx].nama_kelas.c_str())) {
            for (int n = 0; n < daftar_kelas.size(); n++) {
                if (ImGui::Selectable(daftar_kelas[n].nama_kelas.c_str(), (m_laporan_kelas_idx == n)))
                    m_laporan_kelas_idx = n;
            }
            ImGui::EndCombo();
        }
    }

    ImGui::InputText("Tanggal Laporan", m_laporan_tanggal_buffer, IM_ARRAYSIZE(m_laporan_tanggal_buffer));
    
    if (ImGui::Button("Buat Laporan")) {
        tombol_buat_laporan_diklik = true;
    }
    
    ImGui::Text("Status: %s", m_status_laporan.c_str());

    ImGui::End();
}

int UIManager::getLaporanKelasId(const std::vector<Kelas>& daftar_kelas) {
    if (daftar_kelas.empty()) return -1;
    return daftar_kelas[m_laporan_kelas_idx].id;
}
std::string UIManager::getLaporanTanggal() { return std::string(m_laporan_tanggal_buffer); }
void UIManager::setStatusLaporan(const std::string& status) { m_status_laporan = status; }
int UIManager::getLaporanKelasIndex() const {
    return m_laporan_kelas_idx;
}