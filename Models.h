#pragma once
#include <string>
#include <vector>

// Struct sederhana untuk model data Mahasiswa
struct Mahasiswa {
    int id;
    std::string npm;
    std::string nama;
};

// Struct sederhana untuk model data Kelas
struct Kelas {
    int id;
    std::string kode_kelas;
    std::string nama_kelas;
};

// Struct untuk menampung hasil join data kehadiran
struct AbsensiRecord {
    std::string npm;
    std::string nama;
    std::string status;
};