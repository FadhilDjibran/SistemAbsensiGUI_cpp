#include "Aplikasi.h"

// Include standar dan library pihak ketiga
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl3.h"
#include <windows.h>
#include <GL/gl.h>
#include <tchar.h>
using namespace std;

// Deklarasi global yang dibutuhkan oleh boilerplate Win32.
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static HWND hwnd;
static WNDCLASSEX wc;
static HGLRC   g_hRC;
static HDC     g_hDC;
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


// Konstruktor aplikasi
Aplikasi::Aplikasi() : 
    repo(db), 
    ui(), 
    masterDataManager(repo), 
    laporanManager(repo),
    m_show_popup_edit(false),   // Set popup edit tidak aktif di awal
    m_show_popup_hapus(false)  // Set popup hapus tidak aktif di awal
{}

// Fungsi utama 
void Aplikasi::run() {
    if (!db.connect("127.0.0.1", "root", "", "db_absensi", 3306)) {
        return; // Jika gagal, aplikasi berhenti
    }
    repo.setupStrukturAwal();
    load_initial_data();
    init_gui();
    main_loop();
    shutdown_gui();
    db.disconnect();
}

// Memuat data awal dari database
void Aplikasi::load_initial_data() {
    daftar_mahasiswa = repo.ambilSemuaMahasiswa();
    daftar_kelas = repo.ambilSemuaKelas();
    for(const auto& mhs : daftar_mahasiswa) {
        status_kehadiran[mhs.id] = 0;
    }
}

// Memuat data absensi yang sudah tersimpan
void Aplikasi::muatDataKehadiranTersimpan() {
    int kelas_id = ui.getKelasIdTerpilih(daftar_kelas);
    string tanggal = ui.getTanggalTerpilih();
    data_kehadiran_tersimpan = repo.ambilDataKehadiran(kelas_id, tanggal);
}

// Menyimpan data absensi
void Aplikasi::simpan_data_absensi() {
    int kelas_id = ui.getKelasIdTerpilih(daftar_kelas);
    string tanggal = ui.getTanggalTerpilih();
    if (kelas_id != -1 && !tanggal.empty()) {
        if (repo.simpanAbsensi(kelas_id, tanggal, status_kehadiran)) {
            ui.setStatusSimpan("Sukses! Semua data absensi telah disimpan.");
        } else {
            ui.setStatusSimpan("Gagal menyimpan beberapa data absensi.");
        }
    }
}

// logika penambahan mahasiswa baru
void Aplikasi::tambahMahasiswaBaru() {
    string npm = ui.getNpmInput();
    string nama = ui.getNamaInput();
    if (masterDataManager.tambahMahasiswa(npm, nama)) {
        ui.setStatusTambahMhs("Sukses! Mahasiswa '" + nama + "' ditambahkan.");
        load_initial_data(); // Muat ulang daftar mahasiswa setelah ditambah
    } else {
        ui.setStatusTambahMhs("Gagal! NPM mungkin sudah ada atau input kosong.");
    }
}

// logika pengubahan data mahasiswa
void Aplikasi::ubahMahasiswaTerpilih() {
    if (masterDataManager.ubahMahasiswa(m_mhs_dipilih.id, m_mhs_dipilih.npm, m_mhs_dipilih.nama)) {
        load_initial_data(); // Muat ulang daftar mahasiswa setelah ditambah
    }
}

// logika penghapusan mahasiswa
void Aplikasi::hapusMahasiswaTerpilih() {
    if (masterDataManager.hapusMahasiswa(m_mhs_dipilih.id)) {
        load_initial_data(); // Muat ulang daftar mahasiswa setelah ditambah
    }
}

// logika pembuatan laporan 
void Aplikasi::buatLaporanBaru() {
    int kelas_idx = ui.getLaporanKelasIndex();
    if (kelas_idx < 0 || kelas_idx >= daftar_kelas.size()) {
        ui.setStatusLaporan("Gagal: Kelas tidak valid.");
        return;
    }
    int kelas_id = daftar_kelas[kelas_idx].id;
    string kode_kelas = daftar_kelas[kelas_idx].kode_kelas;
    string tanggal = ui.getLaporanTanggal();
    string nama_file = "Laporan_" + kode_kelas + "_" + tanggal + ".csv";
    
    if (laporanManager.buatLaporanAbsensi(nama_file, kelas_id, tanggal)) {
        ui.setStatusLaporan("Sukses! Laporan disimpan sebagai " + nama_file);
    } else {
        ui.setStatusLaporan("Gagal membuat laporan. Tidak ada data pada tanggal tersebut.");
    }
}


// Loop utama 
void Aplikasi::main_loop() {
    bool done = false;
    int id_kelas_sebelumnya = -1;
    string tanggal_sebelumnya = "";
    muatDataKehadiranTersimpan();
    id_kelas_sebelumnya = ui.getKelasIdTerpilih(daftar_kelas);
    tanggal_sebelumnya = ui.getTanggalTerpilih();

    static int tab_aktif = 0;

    while (!done) {
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg); ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT) done = true;
        }
        if (done) break;

        ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplWin32_NewFrame(); ImGui::NewFrame();
        
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Menu")) {
                if (ImGui::MenuItem("Absensi", NULL, tab_aktif == 0)) { tab_aktif = 0; }
                if (ImGui::MenuItem("Manajemen Mahasiswa", NULL, tab_aktif == 1)) { tab_aktif = 1; }
                if (ImGui::MenuItem("Laporan", NULL, tab_aktif == 2)) { tab_aktif = 2; }
                ImGui::Separator();
                if (ImGui::MenuItem("Keluar")) { done = true; }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (tab_aktif == 0) {
            // Logika untuk Tab Absensi
            int id_kelas_sekarang = ui.getKelasIdTerpilih(daftar_kelas);
            string tanggal_sekarang = ui.getTanggalTerpilih();
            if (id_kelas_sekarang != id_kelas_sebelumnya || tanggal_sekarang != tanggal_sebelumnya) {
                muatDataKehadiranTersimpan();
                id_kelas_sebelumnya = id_kelas_sekarang;
                tanggal_sebelumnya = tanggal_sekarang;
            }
            bool tombol_simpan_diklik = false;
            ui.render(daftar_kelas, daftar_mahasiswa, status_kehadiran, tombol_simpan_diklik, data_kehadiran_tersimpan);
            if(tombol_simpan_diklik) {
                simpan_data_absensi();
                muatDataKehadiranTersimpan();
            }
        } 
        else if (tab_aktif == 1) {
            // Logika untuk Tab Manajemen Mahasiswa
            bool tombol_tambah_diklik = false;
            int id_edit = -1, id_hapus = -1;
            ui.renderManajemenMahasiswa(daftar_mahasiswa, tombol_tambah_diklik, id_edit, id_hapus);
            
            if (tombol_tambah_diklik) { tambahMahasiswaBaru(); }
            
            // Tombol 'Ubah' diklik untuk suatu ID
            if (id_edit != -1) {
                for(const auto& mhs : daftar_mahasiswa) { if(mhs.id == id_edit) { m_mhs_dipilih = mhs; break; } }
                m_show_popup_edit = true; // flag popup
            }
            // Tombol 'Hapus' diklik untuk suatu ID
            if (id_hapus != -1) {
                for(const auto& mhs : daftar_mahasiswa) { if(mhs.id == id_hapus) { m_mhs_dipilih = mhs; break; } }
                m_show_popup_hapus = true; // flag popup
            }
        }
        else if (tab_aktif == 2) {
            // Logika untuk Tab Laporan
            bool tombol_buat_laporan_diklik = false;
            ui.renderLaporan(daftar_kelas, tombol_buat_laporan_diklik);
            if(tombol_buat_laporan_diklik) {
                buatLaporanBaru();
            }
        }

        // Logika popup
        bool tombol_simpan_perubahan_diklik = false;
        ui.renderPopupEditMahasiswa(m_show_popup_edit, m_mhs_dipilih, tombol_simpan_perubahan_diklik);
        if (tombol_simpan_perubahan_diklik) {
            ubahMahasiswaTerpilih();
        }

        bool tombol_konfirmasi_hapus_diklik = false;
        ui.renderPopupHapusMahasiswa(m_show_popup_hapus, m_mhs_dipilih, tombol_konfirmasi_hapus_diklik);
        if (tombol_konfirmasi_hapus_diklik) {
            hapusMahasiswaTerpilih();
        }
        
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SwapBuffers(g_hDC);
    }
}


// Inisialisasi semua boilerplate Window dan GUI 
void Aplikasi::init_gui() {
    wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Aplikasi Absensi OOP"), NULL };
    ::RegisterClassEx(&wc);
    hwnd = ::CreateWindow(wc.lpszClassName, _T("Aplikasi Absensi Mahasiswa (OOP)"), WS_OVERLAPPEDWINDOW, 100, 100, 950, 600, NULL, NULL, wc.hInstance, NULL);
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd); pfd.nVersion = 1; pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA; pfd.cColorBits = 24; pfd.cDepthBits = 16; pfd.iLayerType = PFD_MAIN_PLANE;
    g_hDC = GetDC(hwnd); int pixelFormat = ChoosePixelFormat(g_hDC, &pfd);
    SetPixelFormat(g_hDC, pixelFormat, &pfd); g_hRC = wglCreateContext(g_hDC); wglMakeCurrent(g_hDC, g_hRC);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(hwnd); ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();
    ::ShowWindow(hwnd, SW_SHOWDEFAULT); ::UpdateWindow(hwnd);
}

// Bersihkan semua resource GUI saat aplikasi ditutup
void Aplikasi::shutdown_gui() {
    ImGui_ImplOpenGL3_Shutdown(); ImGui_ImplWin32_Shutdown(); ImGui::DestroyContext();
    wglDeleteContext(g_hRC); ::DestroyWindow(hwnd); ::UnregisterClass(wc.lpszClassName, wc.hInstance);
}

// Boilerplate Win32 Message Handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;
    switch (msg) {
        case WM_DESTROY: ::PostQuitMessage(0); return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}