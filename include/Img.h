#include <array>
#include <vector>
#include <filesystem>
#include <optional>
#include <string>
#include <memory>

namespace book {
    namespace fs = std::filesystem; // 给 std::filesystem 起个别名

    constexpr std::array<std::string_view, 4> IMG_TYPES = {
        ".jpg", ".png", ".gif", ".webp" };      // 合法的图像文件后缀

    class ImagesManager;

    class ImagesManager {
    public:
        // 默认构造函数
        ImagesManager() = default;
        // 将 path 下所有图像文件都归入管理器
        ImagesManager(const fs::path &path);
        // 使用 images 内储存的所有的图像路径初始化管理器
        ImagesManager(const std::vector<fs::path> &images);
        // 使用 images 内储存的所有的图像路径初始化管理器
        ImagesManager(std::vector<fs::path> &&images);
        // 禁用复制构造
        ImagesManager(const ImagesManager &) = delete;
        // 移动构造函数
        ImagesManager(ImagesManager &&man);

    private:
        std::vector<fs::path> m_images;         // 图像路径

    public:
        // 将当前所有图像文件复制到 destPath 目录下
        // destPath 必须为目录
        // moveOldPath 为 true 时，更新管理器储存的图像路径到新路径
        void copy(const fs::path &destPath, bool moveOldPath = false);
        // 将当前所有图像文件移动到 destPath 目录下
        // destPath 必须为目录
        // 同时更新管理器储存的图像路径
        void move(const fs::path &destPath);
        // 清空管理器
        void clear(bool removeFiles = false);
        // 将第 index 个图像移除管理器（编号从 0 开始）
        // 当 removeFile 为 true 时，将文件从磁盘上删除
        void remove(std::size_t index, bool removeFile = false);
        // 将第 index0 个图像和第 index1 个图像在管理器中的顺序进行交换
        void swap(std::size_t index0, std::size_t index1);
        // 把新的图像添加到管理器里
        void add(const fs::path &imagePath);
        // 获取第 index 个图像的路径
        // 未处理 index 不合法的情况
        const fs::path &getImagePath(std::size_t index) const;
        // 获取第 index 个图像的二进制路径
        // 返回值类型为 std::unique_ptr<std::string>
        // 如果图像路径失效（index不合法或者路径上文件不存在），则返回nullptr
        std::unique_ptr<std::string> getImageContent(std::size_t index) const;
        // 清空管理器，扫描 srcPath 目录下的图像文件并添加到管理器内
        void scanImageFiles(const fs::path &srcPath, bool add = false);

        // 向 file 内写入类
        bool write(std::ofstream &out) const;
        // 从 file 内入读类
        bool read(std::ifstream &file);
        // 获取管理器管理的图像数量
        std::size_t getSumOfImages() const;

    private:
        bool m_checkIndex(std::size_t i) const;
    };
}