#include "MasterDataManager.h"
using namespace std;

MasterDataManager::MasterDataManager(Repository& repo) : m_repo(repo) {}

bool MasterDataManager::tambahMahasiswa(const string& npm, const string& nama) {
    if (npm.empty() || nama.empty()) {
        return false;
    }
    return m_repo.tambahMahasiswa(npm, nama);
}

bool MasterDataManager::ubahMahasiswa(int id, const string& npm, const string& nama) {
    if (npm.empty() || nama.empty()) return false;
    return m_repo.ubahMahasiswa(id, npm, nama);
}

bool MasterDataManager::hapusMahasiswa(int id) {
    return m_repo.hapusMahasiswa(id);
}