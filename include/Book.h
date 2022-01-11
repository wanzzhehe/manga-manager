#include <cstdint>
#include "Tag.h"
#include "Img.h"

namespace book {
    using BookIdType = std::uint32_t;

    class Book : public ImagesManager {
    public:
        // 默认构造函数
        Book() = default;
        // 将 bookPath 目录下所有图像文件加入管理器，tagManager 为标签管理器指针
        Book(const fs::path &bookPath, TagManager *tagManager, BookIdType id);
        // 将 srcPath 目录下所有图像文件添加到 destPath 目录下并加入管理器
        // tagManager 为标签管理器指针
        // 如果 removeOldFile 为 false，那么不删除 srcPath 目录下的图像文件，即对源文件进行复制
        // 如果 removeOldFile 为 true，那么删除 srcPath 目录下的图像文件，即对源文件进行移动 
        Book(const fs::path &srcPath, const fs::path &destPath, TagManager *tagManager,
            BookIdType id, const TagIdList &tags, bool removeOldFile = false);
        // 将 images 里面所有图像文件添加到 destPath 目录下并加入管理器
        // tagManager 为标签管理器指针
        // 如果 removeOldFile 为 false，那么不删除 images 所指向的图像文件，即对源文件进行复制
        // 如果 removeOldFile 为 true，那么删除 images 所指向的图像文件，即对源文件进行移动
        Book(const std::vector<fs::path> &images, const fs::path &destPath, TagManager *tagManager,
            BookIdType id, const TagIdList &tags, bool removeOldFile = false);
        // 由于需要管理文件，删除了复制构造函数
        Book(const Book &) = delete;
        // 移动构造函数
        Book(Book &&book);

    private:
        TagManager *m_tagManager;       // 标签管理器指针，指向该书籍标签所属的标签管理器
        BookIdType m_bookId;            // 漫画ID
        TagIdList m_tags;               // 标签列表

    public:
        // 返回书籍ID
        BookIdType getBookId() const;
        // 获取全部书籍标签ID
        std::unique_ptr<TagIdList> getTags() const;
        // 获取属于标签组 groupId 的所有标签
        std::unique_ptr<TagIdList> getTags(TagIdType groupId) const;
        // 添加标签
        void addTag(TagIdType tagId);
        // 删除标签
        void removeTag(TagIdType tagId);
        // 清除所有属于 groupId 组的标签
        void removeTags(TagIdType groupId);

        // 获取标签数量
        std::size_t getSumOfTags() const;
        // 获取属于 groupId 组的标签的数量
        std::size_t getSumOfTags(TagIdType groupId) const;

        // 从文件输入流 in 中读取，并设置 tagManager 指针
        bool read(std::ifstream &in, TagManager *tagManager);
        // 向文件输出流 out 中输出
        bool write(std::ofstream &out) const;
    };
}