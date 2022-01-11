#include "Img.h"
#include <fstream>

using namespace book;

/* class ImagesManager */
/* ===== BEGIN ===== */
// 构造函数

ImagesManager::ImagesManager(const fs::path &path) {
    scanImageFiles(path);
}

ImagesManager::ImagesManager(const std::vector<fs::path> &images)
: m_images(images) { }

ImagesManager::ImagesManager(std::vector<fs::path> &&images)
: m_images(std::move(images)) { }

// 公有函数

void ImagesManager::copy(const fs::path &destPath, bool moveOldPath) {
    fs::create_directories(destPath);
    m_copy(destPath, moveOldPath);
}

void ImagesManager::move(const fs::path &destPath) {
    fs::create_directories(destPath);
    m_move(destPath);
}

void ImagesManager::clear(bool removeFiles) {
    if (removeFiles) {
        for (const auto &path : m_images) {
            fs::remove(path);
        }
    }
    m_images.clear();
}

void ImagesManager::remove(std::size_t index, bool removeFile) {
    if (!m_checkIndex(index)) return ;
    if (removeFile) fs::remove(m_images.at(index));
    for (auto i = m_images.begin() + index + 1; i != m_images.end(); ++i) {
        *(i - 1) = *i;
    }
    m_images.pop_back();
}

void ImagesManager::swap(std::size_t index0, std::size_t index1) {
    if (!m_checkIndex(index0) || !m_checkIndex(index1)) return ;
    m_images.at(index0).swap(m_images.at(index1));
}

void ImagesManager::add(const fs::path &imagePath) {
    if (!fs::exists(imagePath) || !fs::is_regular_file(imagePath)) return ;
    fs::path path(fs::canonical(imagePath));
    m_images.emplace_back(std::move(path));
}

const fs::path &ImagesManager::getImagePath(std::size_t index) const {
    return m_images.at(index);
}

std::unique_ptr<std::string> ImagesManager::getImageContent(std::size_t index) const {
    if (!m_checkIndex(index)) return nullptr;

    const auto &path = m_images.at(index);
    auto size = fs::file_size(path);
    std::ifstream fin(path, std::ios::in | std::ios::binary);
    if (fin.fail()) return nullptr;

    std::unique_ptr<std::string> ret(new std::string(size, '\x00'));
    fin.read(std::addressof((*ret)[0]), size); fin.close();
    return ret;
}

void ImagesManager::scanImageFiles(const fs::path &srcPath, bool add) {
    if (!fs::exists(srcPath) || !fs::is_directory(srcPath)) return ;

    if (!add) m_images.clear();
    for (auto &i : fs::directory_iterator(srcPath)) {
        if (!i.is_regular_file()) continue;
        auto &path = i.path();
        for (auto type : IMG_TYPES) {
            if (type != path.extension()) continue;
            m_images.emplace_back(path);
        }
    }
}

bool ImagesManager::write(std::ofstream &out) const {
    std::size_t size = m_images.size(), length;
    out.write(reinterpret_cast<const char *>(&size), sizeof(size));
    for (auto &path : m_images) {
        auto &tmp = path.native();
        std::string ret;
        if constexpr (std::is_same_v<fs::path::string_type, std::wstring>) {
            // 被迫用宏再包裹一层 _^_
#ifdef _WIN32
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            ret = converter.to_bytes(tmp);
#endif
        } else if constexpr (std::is_same_v<fs::path::string_type, std::string>) {
            // FIXIT TOMORROW
            // FIXED
#ifdef __linux
            ret = tmp;
#endif
        } else return false;
        length = ret.length();
        out.write(reinterpret_cast<const char*>(&length), sizeof(length));
        out.write(ret.c_str(), ret.size());
    }
    return true;
}

bool ImagesManager::read(std::ifstream& in) {
    std::size_t size, length;
    std::string tmp;
    in.read(reinterpret_cast<char*>(&size), sizeof(size));
    for (std::size_t i = 0; i < size; ++i) {

        in.read(reinterpret_cast<char*>(&length), sizeof(length));
        tmp.resize(length);
        in.read(reinterpret_cast<char*>(&tmp[0]), length);

        if constexpr (std::is_same_v<fs::path::string_type, std::wstring>) {
#ifdef _WIN32
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            m_images.emplace_back(converter.from_bytes(tmp));
#endif
        } else if constexpr (std::is_same_v<fs::path::string_type, std::string>) {
#ifdef __linux
            m_images.emplace_back(tmp);
#endif
        } else return false;
    }
    return true;
}

// 私有函数

/* ====== END ====== */