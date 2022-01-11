#include "Book.h"

using namespace book;

/* class Book */
/* ===== BEGIN ===== */
// 构造函数

// 将 bookPath 目录下所有图像文件加入管理器，tagManager 为标签管理器指针
Book::Book(const fs::path &bookPath, TagManager *tagManager, BookIdType id, const TagIdList &tags)
    : ImagesManager(), m_tagManager(tagManager), m_bookId(id), m_tags(tags) {
    scanImageFiles(bookPath);
}

// 将 srcPath 目录下所有图像文件添加到 destPath 目录下并加入管理器
// tagManager 为标签管理器指针
// 如果 removeOldFile 为 false，那么不删除 srcPath 目录下的图像文件，即对源文件进行复制
// 如果 removeOldFile 为 true，那么删除 srcPath 目录下的图像文件，即对源文件进行移动 
Book::Book(const fs::path &srcPath, const fs::path &destPath, TagManager *tagManager,
    BookIdType id, const TagIdList &tags, bool removeOldFile = false)
    : ImagesManager(srcPath), m_tagManager(tagManager), m_bookId(id), m_tags(tags) {
    if (removeOldFile) move(destPath);
    else copy(destPath, true);
}

// 将 images 里面所有图像文件添加到 destPath 目录下并加入管理器
// tagManager 为标签管理器指针
// 如果 removeOldFile 为 false，那么不删除 images 所指向的图像文件，即对源文件进行复制
// 如果 removeOldFile 为 true，那么删除 images 所指向的图像文件，即对源文件进行移动
Book::Book(const std::vector<fs::path> &images, const fs::path &destPath, TagManager *tagManager,
    BookIdType id, const TagIdList &tags, bool removeOldFile = false)
    : ImagesManager(images), m_tagManager(tagManager), m_bookId(id), m_tags(tags) {
    if (removeOldFile) move(destPath);
    else copy(destPath, true);
}

// 移动构造函数
Book::Book(Book &&book) : ImagesManager(std::move(book)), m_tagManager(book.m_tagManager),
    m_bookId(book.m_bookId), m_tags(std::move(book.m_tags)) {
    book.m_tagManager = nullptr;
    book.m_bookId = nullBookId;
}

BookIdType Book::getBookId() const {
    return m_bookId;
}

std::unique_ptr<TagIdList> Book::getTags() const {
    return std::make_unique<TagIdList>(new TagIdList(m_tags));
}

std::unique_ptr<TagIdList> Book::getTags(TagIdType groupId) const {
    std::unique_ptr<TagIdList> ret(new TagIdList());
    for (auto id : m_tags) {
        auto &tag = m_tagManager->getBookTag(id);
        if (tag.isNull() || tag.getGroupId() != groupId) continue;
        ret->emplace_back(id);
    }
    return ret;
}

void Book::addTag(TagIdType tagId) {
    if (!m_tagManager->checkTagId(tagId)) return ;
    m_tags.push_back(tagId);
}

void Book::removeTag(TagIdType tagId) {
    if (!m_tagManager->checkTagId(tagId)) return ;

    auto it = m_tags.begin();
    for (; it != m_tags.end() && *it != tagId; ++it);
    if (it == m_tags.end()) return ;
    for (++it; it != m_tags.end(); ++it) {
        *(it - 1) = *it;
    }
    m_tags.pop_back();
}

void Book::removeTags(TagIdType groupId) {
    if (!m_tagManager->checkGroupTagId(groupId)) return ; 

    auto it0 = m_tags.begin(), it1 = m_tags.begin();
    std::size_t cnt = 0;
    for (; it0 != m_tags.end() && it1 != m_tags.end(); ++it0) {
        for (; it1 != m_tags.end() && m_tagManager->getGroupTagId(*it1) == groupId; ++it1);
        *it0 = *it1;
        ++cnt;
    }
    while (cnt--) m_tags.pop_back();
}

std::size_t Book::getSumOfTags() const {
    return m_tags.size();
}

std::size_t Book::getSumOfTags(TagIdType groupId) const {
    std::size_t cnt = 0;
    for (auto id : m_tags) {
        if (m_tagManager->getGroupTagId(id) == groupId) ++cnt;
    }
    return cnt;
}
/* ====== END ====== */