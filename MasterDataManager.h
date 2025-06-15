#pragma once
#include "Repository.h"
#include <string>

class MasterDataManager {
private:
    Repository& m_repo;

public:
    MasterDataManager(Repository& repo);
    bool tambahMahasiswa(const std::string& npm, const std::string& nama);
    bool ubahMahasiswa(int id, const std::string& npm, const std::string& nama);
    bool hapusMahasiswa(int id);
};