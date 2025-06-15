#include "MasterDataManager.h"

MasterDataManager::MasterDataManager(Repository& repo) : m_repo(repo) {}

bool MasterDataManager::tambahMahasiswa(const std::string& npm, const std::string& nama) {
    if (npm.empty() || nama.empty()) {
        return false;
    }
    return m_repo.tambahMahasiswa(npm, nama);
}