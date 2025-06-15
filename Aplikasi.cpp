#include "Aplikasi.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl3.h"
#include <windows.h>
#include <GL/gl.h>
#include <tchar.h>

// Deklarasi global yang dibutuhkan oleh boilerplate Win32.
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static HWND hwnd;
static WNDCLASSEX wc;
static HGLRC   g_hRC;
static HDC     g_hDC;
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


// Konstruktor: Menginisialisasi semua kelas manager dengan referensi ke Repository
Aplikasi::Aplikasi() : 
    repo(db), 
    ui(), 
    masterDataManager(repo), 
    laporanManager(repo) 
{}

// Fungsi utama yang menjalankan seluruh alur aplikasi
void Aplikasi::run() {
    // 1. Hubungkan ke Database
    if (!db.connect("127.0.0.1", "root", "", "db_absensi", 3306)) {
        return; // Gagal konek, hentikan aplikasi
    }

    // 2. Siapkan struktur tabel dan isi data awal
    repo.setupStrukturAwal();
    
    // 3. Muat data awal dari DB ke memori
    load_initial_data();
    
    // 4. Inisialisasi jendela GUI
    init_gui();

    // 5. Jalankan loop utama aplikasi
    main_loop();
    
    // 6. Matikan GUI dan putuskan koneksi DB setelah aplikasi ditutup
    shutdown_gui();
    db.disconnect();
}

// Memuat daftar mahasiswa & kelas saat aplikasi pertama kali jalan
void Aplikasi::load_initial_data() {
    daftar_mahasiswa = repo.ambilSemuaMahasiswa();
    daftar_kelas = repo.ambilSemuaKelas();
    
    // Set status default untuk semua mahasiswa ke "Hadir"
    for(const auto& mhs : daftar_mahasiswa) {
        status_kehadiran[mhs.id] = 0;
    }
}

// Memuat data absensi yang sudah tersimpan dari DB berdasarkan pilihan UI
void Aplikasi::muatDataKehadiranTersimpan() {
    int kelas_id = ui.getKelasIdTerpilih(daftar_kelas);
    std::string tanggal = ui.getTanggalTerpilih();
    data_kehadiran_tersimpan = repo.ambilDataKehadiran(kelas_id, tanggal);
}

// Menyimpan data absensi dari UI ke database
void Aplikasi::simpan_data_absensi() {
    int kelas_id = ui.getKelasIdTerpilih(daftar_kelas);
    std::string tanggal = ui.getTanggalTerpilih();
    
    if (kelas_id != -1 && !tanggal.empty()) {
        if (repo.simpanAbsensi(kelas_id, tanggal, status_kehadiran)) {
            ui.setStatusSimpan("Sukses! Semua data absensi telah disimpan.");
        } else {
            ui.setStatusSimpan("Gagal menyimpan beberapa data absensi.");
        }
    }
}

// Handler untuk logika penambahan mahasiswa baru
void Aplikasi::tambahMahasiswaBaru() {
    std::string npm = ui.getNpmInput();
    std::string nama = ui.getNamaInput();
    if (masterDataManager.tambahMahasiswa(npm, nama)) {
        ui.setStatusTambahMhs("Sukses! Mahasiswa '" + nama + "' ditambahkan.");
        load_initial_data(); // Penting: Muat ulang daftar mahasiswa agar langsung tampil di tab absensi
    } else {
        ui.setStatusTambahMhs("Gagal! NPM mungkin sudah ada atau input kosong.");
    }
}

// Handler untuk logika pembuatan laporan baru
void Aplikasi::buatLaporanBaru() {
    int kelas_idx = ui.getLaporanKelasIndex(); // Menggunakan index untuk nama file
    int kelas_id = ui.getLaporanKelasId(daftar_kelas);
    std::string tanggal = ui.getLaporanTanggal();

    if(kelas_idx < 0 || kelas_idx >= daftar_kelas.size()) {
        ui.setStatusLaporan("Gagal: Kelas tidak valid.");
        return;
    }

    std::string nama_file = "Laporan_" + daftar_kelas[kelas_idx].kode_kelas + "_" + tanggal + ".csv";
    
    if (laporanManager.buatLaporanAbsensi(nama_file, kelas_id, tanggal)) {
        ui.setStatusLaporan("Sukses! Laporan disimpan sebagai " + nama_file);
    } else {
        ui.setStatusLaporan("Gagal membuat laporan. Tidak ada data pada tanggal tersebut.");
    }
}

// Loop utama tempat semua interaksi dan penggambaran terjadi setiap frame
void Aplikasi::main_loop() {
    bool done = false;
    
    int id_kelas_sebelumnya = -1;
    std::string tanggal_sebelumnya = "";
    
    muatDataKehadiranTersimpan();
    id_kelas_sebelumnya = ui.getKelasIdTerpilih(daftar_kelas);
    tanggal_sebelumnya = ui.getTanggalTerpilih();

    static int tab_aktif = 0; // 0=Absensi, 1=Manajemen Mahasiswa, 2=Laporan

    while (!done) {
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done) break;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        
        // --- Membuat Main Menu Bar untuk Navigasi ---
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

        // --- Render Tampilan GUI berdasarkan Tab yang Aktif ---
        if (tab_aktif == 0) {
            // Tampilan Absensi Utama
            int id_kelas_sekarang = ui.getKelasIdTerpilih(daftar_kelas);
            std::string tanggal_sekarang = ui.getTanggalTerpilih();
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
            // Tampilan Manajemen Mahasiswa
            bool tombol_tambah_mhs_diklik = false;
            ui.renderManajemenMahasiswa(tombol_tambah_mhs_diklik);
            if (tombol_tambah_mhs_diklik) {
                tambahMahasiswaBaru();
            }
        }
        else if (tab_aktif == 2) {
            // Tampilan Laporan
            bool tombol_buat_laporan_diklik = false;
            ui.renderLaporan(daftar_kelas, tombol_buat_laporan_diklik);
            if(tombol_buat_laporan_diklik) {
                buatLaporanBaru();
            }
        }
        
        ImGui::Render();
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SwapBuffers(g_hDC);
    }
}

// Fungsi untuk inisialisasi semua boilerplate Window dan GUI
void Aplikasi::init_gui() {
    wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("Aplikasi Absensi OOP"), NULL };
    ::RegisterClassEx(&wc);
    hwnd = ::CreateWindow(wc.lpszClassName, _T("Aplikasi Absensi Mahasiswa (OOP)"), WS_OVERLAPPEDWINDOW, 100, 100, 1200, 720, NULL, NULL, wc.hInstance, NULL);

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

// Fungsi untuk membersihkan semua resource GUI saat aplikasi ditutup
void Aplikasi::shutdown_gui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    wglDeleteContext(g_hRC);
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);
}

// Boilerplate Win32 Message Handler (tidak perlu diubah)
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
    switch (msg) {
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}