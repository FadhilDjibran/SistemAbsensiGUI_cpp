#pragma once
#include <string>
#include <vector>

class Mahasiswa {
public: 
    int id;
    std::string npm;
    std::string nama;
};

class Kelas {
public: 
    int id;
    std::string kode_kelas;
    std::string nama_kelas;
};

class AbsensiRecord {
public: 
    std::string npm;
    std::string nama;
    std::string status;
};