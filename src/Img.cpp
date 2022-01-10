#include "Img.h"
#include <fstream>

using namespace book;

/* class ImagesManager */
/* ===== BEGIN ===== */
// 构造函数

ImagesManager::ImagesManager(const fs::path& path) {
    if (!fs::exists(path)) return;
    if (!fs::is_directory(path)) return;
    m_curDir = fs::canonical(path);
    m_scanImageFiles();
}

ImagesManager::ImagesManager(const fs::path& srcPath, const fs::path& destPath, bool removeOldFile, bool removeOldDirectory) {
    if (!fs::exists(srcPath)) return ;
    if (!fs::is_directory(srcPath)) return ;
    if (fs::exists(destPath)) return ;
    chdir(srcPath);
    if (removeOldFile) move(destPath, removeOldDirectory);
    else {
        copy(destPath);
        chdir(destPath);
    }
}

// 移动构造函数

ImagesManager::ImagesManager(ImagesManager&& man) :
    m_curDir(std::move(man.m_curDir)), m_imagesList(std::move(m_imagesList))
{ }

// 公有函数

void ImagesManager::move(const fs::path& path, bool removeOldDirectory) {
    copy(path);
    if (removeOldDirectory) fs::remove_all(m_curDir);
    else {
        for (std::size_t i = 0; i < m_imagesList.size(); ++i) {
            if (!checkIfImageExist(i)) continue;
            const auto &img = m_imagesList.at(i);
            fs::remove(m_curDir / img);
        }
    }
    chdir(path);
}

void ImagesManager::copy(const fs::path &path) {
}

// 私有函数

/* ====== END ====== */