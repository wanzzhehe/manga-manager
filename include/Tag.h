#include <cstdint>
#include <limits>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <memory>

namespace book {
    /* 自定义类型 */
    using TagIdType = uint16_t;                 // 标签ID类型
    using TagIdList = std::vector<TagIdType>;   // 标签ID列表类型

    /* 类的提前声明 */
    class Tag;
    class BookTag;
    class GroupTag;
    class TagManager;

    /* 一些常量 */
    constexpr TagIdType nullTagId = std::numeric_limits<TagIdType>::min();      // 空标签ID
    constexpr TagIdType maxTagId = std::numeric_limits<TagIdType>::max();       // 最大标签ID

    extern const BookTag nullbtag;      // 空书标签
    extern const GroupTag nullgtag;     // 空组标签

    // concept 用于判断类型是否是 BookTag 和 GroupTag 之一
    template<typename TagType>
    concept isTagType = std::is_same_v<TagType, BookTag> || std::is_same_v<TagType, GroupTag>;

    /* class Tag */
    // 标签基类
    class Tag {
    public:
        // 构造函数
        Tag();
        // 构造函数
        Tag(TagIdType id, std::string_view name);

    public:
        /*
         * 声明 TagManager 为 Tag 的友元类
         * 用于 TagManager::m_eraseTag 内对 m_id 成员置空时使用
         * 实际上，由于不想暴露 Tag 的修改权限，所以不得不使用这种有一定侵入性的方法
         * 但是感觉依旧不爽，等到发现更好方法之后再修改
         */
        friend class book::TagManager;

    private:
        // 成员变量
        TagIdType m_id;         // 标签ID
        std::string m_name;     // 标签名

    public:
        // 标签从文件流读入
        void read(std::ifstream &in);
        // 标签向文件流写入
        void write(std::ofstream &out) const;

        // 获取标签ID
        TagIdType getId() const;
        // 获取标签名
        std::string_view getName() const;
    };

    /* class BookTag */
    // Tag 的派生类，为书籍标签
    class BookTag final : public Tag {
    public:
        // 构造函数
        BookTag();
        // 构造函数
        BookTag(TagIdType bookTagId, TagIdType groupTagId, std::string_view name);

    private:
        // 新增成员变量
        TagIdType m_groupId;    // 书标签所属标签组ID

    public:
        // 成员函数

        // 获取标签组ID
        TagIdType getGroupId() const;
        // 标签从文件流读入
        void read(std::ifstream &in);
        // 标签向文件流写入
        void write(std::ofstream &out) const;
    };

    /* class GroupTag */
    // Tag 的派生类，与 Tag 几乎没有区别，为标签的分组标签
    class GroupTag final : public Tag {
    public:
        // 构造函数
        GroupTag();
        // 构造函数
        GroupTag(TagIdType groupTagId, std::string_view name);
        // 虽然与Tag没有什么区别，构造函数还是要写的
    };

    /*
     * class TagManager
     * 用于管理书标签与组标签
     */
    class TagManager {
    public:
        // 构造函数
        TagManager();
        // 构造函数，从文件中读入构造
        TagManager(std::string_view path);

    private:
        using TagIdHeap = std::priority_queue<TagIdType, std::vector<TagIdType>, std::greater<TagIdType>>; // 储存 TagId 的小根堆

        // 用于储存标签信息的内部类
        template<isTagType TagType>
        struct TagsInfo {
        public:
            // 构造函数
            TagsInfo();

        public:
            std::size_t m_curSumOfTags;         // 当前共有多少标签
            TagIdType m_curMaxTag;              // 当前最大标签ID
            std::vector<TagType> m_Tags;        // 标签列表，保证合法标签ID与此动态数组下标一致
            TagIdHeap m_erasedTags;             // 被删除的标签ID
        };

    private:
        TagsInfo<BookTag> m_bookTags;           // 书标签信息
        TagsInfo<GroupTag> m_groupTags;         // 组标签信息

    public:
        // 清空 TagManager 的信息
        void clear();

        // 写入指定路径
        void read(std::string_view path);
        // 从指定路径读入
        void write(std::string_view path) const;

        void read(std::ifstream &in);
        void write(std::ofstream &out) const;

        /*
         * 用标签名获取书ID
         * 如果标签存在，则返回对应标签ID
         * 否则返回空标签ID，即 nullTagId
         */
        TagIdType getBookTagId(std::string_view name) const;
        /*
         * 用标签名获取组ID
         * 如果标签存在，则返回对应标签ID
         * 否则返回空标签ID，即 nullTagId
         */
        TagIdType getGroupTagId(std::string_view name) const;
        /*
         * 用书标签ID获取书标签
         * 如果标签ID合法，则返回标签的常量引用
         * 否则返回空书标签，即 nullbtag
         */
        const BookTag &getBookTag(TagIdType id) const;
        /*
         * 用组标签ID获取组标签
         * 如果标签ID合法，则返回标签的常量引用
         * 否则返回空组标签，即 nullgtag
         */
        const GroupTag &getGroupTag(TagIdType id) const;
        /*
         * 检查书标签ID是否有效
         * 有效返回 true，无效返回 false
         */
        bool checkIfValidBookTagId(TagIdType id) const;
        /*
         * 检查组标签ID是否有效
         * 有效返回 true，无效返回 false
         */
        bool checkIfValidGroupTagId(TagIdType id) const;
        /*
         * 创建一个新的书标签
         * 返回新创建标签的ID
         * 如果创建失败（标签已满或者标签已存在），则返回空ID，即 nullTagId
         */
        TagIdType createBookTag(std::string_view name, TagIdType groupTagId);
        /*
         * 创建一个新的组标签
         * 返回新创建标签的ID
         * 如果创建失败（标签已满或者标签已存在），则返回空ID，即 nullTagId
         */
        TagIdType createGroupTag(std::string_view name);
        /*
         * 删除书标签
         * 成功返回 true，失败返回 false
         */
        bool eraseBookTag(TagIdType bookTagId);
        /*
         * 删除组标签
         * 成功返回 true，失败返回 false
         */
        bool eraseGroupTag(TagIdType groupTagId);
        /*
         * 获取当前书标签个数
         * 返回个数
         */
        std::size_t getSumOfBookTags() const;
        /*
         * 获取当前组标签个数
         * 返回个数
         */
        std::size_t getSumOfGroupTags() const;
        /*
         * 获取某一个标签组下的书标签数量
         */
        std::size_t getSumOfTagsFromGroup(TagIdType groupId) const;
        /*
         * 获取某个标签组下的所有书标签
         */
        std::unique_ptr<TagIdList> getBookTagsFromGroup(TagIdType groupId) const;

    private:
        /* 通用清空 Info */
        template<isTagType TagType>
        void m_clearTagsInfo(TagsInfo<TagType> &info);
        /* 检查 id 再 info 内是否有效 */
        template<isTagType TagType>
        bool m_checkIfValidId(TagIdType id, const TagsInfo<TagType> &info) const;
        /* 通过 name 获取 info 内标签ID */
        template<isTagType TagType>
        TagIdType m_getTagId(std::string_view name, const TagsInfo<TagType> &info) const;
        /* 通过 id 获取 info 内标签 */
        template<isTagType TagType>
        const TagType &m_getTag(TagIdType id, const TagsInfo<TagType> &info) const;
        /* 从输入流 in 中读取 info */
        template<isTagType TagType>
        void m_readInfo(std::ifstream &in, TagsInfo<TagType> &info);
        /* 将 info 写入输出流 out */
        template<isTagType TagType>
        void m_writeInfo(std::ofstream &out, const TagsInfo<TagType> &info) const;
        /*
         * 从 info 中获取未被使用的新 ID
         * 获取新 ID 即创建了新的标签
         */
        template<isTagType TagType>
        TagIdType m_getNewId(TagsInfo<TagType> &info);
        /* 从 info 中删除标签ID为 id 的标签 */
        template<isTagType TagType>
        bool m_eraseTag(TagIdType id, TagsInfo<TagType> &info);
        /* 获取 info 中标签个数 */
        template<isTagType TagType>
        std::size_t m_getSumOfTags(const TagsInfo<TagType> &info) const;
    };
}