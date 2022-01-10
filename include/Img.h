#include <array>
#include <vector>
#include <filesystem>
#include <optional>
#include <string>
#include <memory>

namespace book {
    namespace fs = std::filesystem;

    constexpr std::array<std::string_view, 4> IMG_TYPES = {
        ".jpg", ".png", ".gif", ".webp"
    };

    class ImagesManager;

    class ImagesManager final {
    public:
        ImagesManager() = default;
        ImagesManager(const fs::path& path);
        ImagesManager(const fs::path& srcPath, const fs::path& destPath, bool removeOldFile = false, bool removeOldDirectory = false);
        ImagesManager(const ImagesManager&) = delete;
        ImagesManager(ImagesManager&& man);

    private:
        fs::path m_curDir;
        std::vector<fs::path> m_imagesList;

    public:
        /* 文件管理部分 */

        void move(const fs::path& destPath, bool removeOldDirectory = false);
        void copy(const fs::path& destPath);
        void remove();
        void remove(std::size_t index);
        void rename(std::size_t index, std::string_view newName);
        void chdir(const fs::path &destPath);
        void flush();
        void swap(std::size_t index0, std::size_t index1);

        /* 内容管理部分 */

        bool checkIfValid() const;
        bool checkIfValidIndex(std::size_t index) const;
        bool checkIfImageExist(std::size_t index) const;
        std::unique_ptr<std::string> getImageContent(std::size_t index) const;

    private:
        std::size_t m_scanImageFiles();
    };
}