#include "Tag.h"

using namespace book;

/* ===== BEGIN ===== */
/* ====== END ====== */

/* 常量 */
/* ===== BEGIN ===== */
const auto nullbtag = BookTag();
const auto nullgtag = GroupTag();
/* ====== END ====== */

/* class Tag */
/* ===== BEGIN ===== */
// 构造函数
Tag::Tag() : m_id(nullTagId), m_name() {}
Tag::Tag(TagIdType id, std::string_view name) : m_id(id), m_name(name) {}

// 类内方法
void Tag::read(std::ifstream &in) {
    in.read(reinterpret_cast<char *>(&m_id), sizeof(m_id));
    std::size_t len;
    in.read(reinterpret_cast<char *>(&len), sizeof(len));
    m_name.resize(len);
    in.read(reinterpret_cast<char *>(&m_name[0]), len);
}

void Tag::write(std::ofstream &out) const {
    out.write(reinterpret_cast<const char *>(&m_id), sizeof(m_id));
    auto len = m_name.length();
    out.write(reinterpret_cast<const char *>(&len), sizeof(len));
    out.write(m_name.c_str(), len);
}

TagIdType Tag::getId() const {
    return m_id;
}

std::string_view Tag::getName() const {
    return m_name;
}
/* ====== END ====== */

/* class BookTag */
/* ===== BEGIN ===== */
// 构造函数
BookTag::BookTag() : Tag(), m_groupId(nullTagId) {}

BookTag::BookTag(TagIdType bookTagId, TagIdType groupTagId, std::string_view name) :
    Tag(bookTagId, name), m_groupId(groupTagId) {}

// 类内方法
TagIdType BookTag::getGroupId() const {
    return m_groupId;
}

void BookTag::read(std::ifstream &in) {
    Tag::read(in);
    in.read(reinterpret_cast<char *>(&m_groupId), sizeof(m_groupId));
}

void BookTag::write(std::ofstream &out) const {
    Tag::write(out);
    out.write(reinterpret_cast<const char *>(&m_groupId), sizeof(m_groupId));
}
/* ====== END ====== */

/* class GroupTag */
/* ===== BEGIN ===== */
// 构造函数
GroupTag::GroupTag() : Tag() {}

GroupTag::GroupTag(TagIdType groupTagId, std::string_view name) : Tag(groupTagId, name) {}
/* ====== END ====== */

/* class TagManager */
/* ===== BEGIN ===== */
// 构造函数
TagManager::TagManager() = default;

TagManager::TagManager(std::string_view path) {
    read(path);         // 从文件读入
}

/* struct TagsInfo in TagManager */
/* ===== BEGIN ===== */
// 构造函数
template<isTagType TagType>
TagManager::TagsInfo<TagType>::TagsInfo() :
    m_curSumOfTags(0U), m_curMaxTag(nullTagId), m_Tags(1), m_erasedTags() {}
/* ====== END ====== */

// 类内方法
// 公开方法
void TagManager::clear() {
    m_clearTagsInfo(m_bookTags);
    m_clearTagsInfo(m_groupTags);
}

void TagManager::read(std::string_view path) {
    constexpr auto readMethod = std::ios::in | std::ios::binary;

    clear();
    auto fin = std::ifstream(path.begin(), readMethod);
    m_readInfo(fin, m_bookTags);
    m_readInfo(fin, m_groupTags);
    fin.close();
}

void TagManager::write(std::string_view path) const {
    constexpr auto writeMethod = std::ios::out | std::ios::binary;

    auto fout = std::ofstream(path.begin(), writeMethod);
    m_writeInfo(fout, m_bookTags);
    m_writeInfo(fout, m_groupTags);
    fout.close();
}

TagIdType TagManager::getBookTagId(std::string_view name) const {
    return m_getTagId(name, m_bookTags);
}

TagIdType TagManager::getGroupTagId(std::string_view name) const {
    return m_getTagId(name, m_groupTags);
}

const BookTag &TagManager::getBookTag(TagIdType id) const {
    return m_getTag(id, m_bookTags);
}

const GroupTag &TagManager::getGroupTag(TagIdType id) const {
    return m_getTag(id, m_groupTags);
}

bool TagManager::checkIfValidBookTagId(TagIdType id) const {
    return m_checkIfValidId(id, m_bookTags);
}

bool TagManager::checkIfValidGroupTagId(TagIdType id) const {
    return m_checkIfValidId(id, m_groupTags);
}

TagIdType TagManager::createBookTag(std::string_view name, TagIdType groupId) {
    auto id = m_getTagId(name, m_bookTags);
    if (id != nullTagId) return id;
    id = m_getNewId(m_bookTags);
    if (id == nullTagId) return id;
    m_bookTags.m_Tags[id] = BookTag(id, groupId, name);
    return id;
}

TagIdType TagManager::createGroupTag(std::string_view name) {
    auto id = m_getTagId(name, m_groupTags);
    if (id != nullTagId) return nullTagId;
    id = m_getNewId(m_groupTags);
    if (id == nullTagId) return id;
    m_groupTags.m_Tags[id] = GroupTag(id, name);
    return id;
}

bool TagManager::eraseBookTag(TagIdType id) {
    return m_eraseTag(id, m_bookTags);
}

bool TagManager::eraseGroupTag(TagIdType id) {
    return m_eraseTag(id, m_groupTags);
}

std::size_t TagManager::getSumOfBookTags() const {
    return m_getSumOfTags(m_bookTags);
}

std::size_t TagManager::getSumOfGroupTags() const {
    return m_getSumOfTags(m_groupTags);
}

std::size_t TagManager::getSumOfTagsFromGroup(TagIdType groupId) const {
    std::size_t ret = 0U;
    for (const auto &tag : m_bookTags.m_Tags) {
        if (tag.getId() == nullTagId) continue;
        if (tag.getGroupId() == groupId) ++ret;
    }
    return ret;
}

std::unique_ptr<TagIdList> TagManager::getBookTagsFromGroup(TagIdType groupId) const {
    std::unique_ptr<TagIdList> ret(new std::vector<TagIdType>());
    for (const auto &tag : m_bookTags.m_Tags) {
        if (tag.getId() == nullTagId) continue;
        if (tag.getGroupId() == groupId) ret->emplace_back(tag.getId());
    }
    return ret;
}

// 私有方法
template<isTagType TagType>
void TagManager::m_clearTagsInfo(TagsInfo<TagType> &info) {
    info.m_curSumOfTags = std::size_t(0U);
    info.m_curMaxTag = nullTagId;
    info.m_Tags.clear();
    info.m_erasedTags = TagIdHeap();
}

template<isTagType TagType>
bool TagManager::m_checkIfValidId(TagIdType id, const TagsInfo<TagType> &info) const {
    return !(id == nullTagId || id > info.m_curMaxTag || info.m_Tags[id].m_id == nullTagId);
}

template<isTagType TagType>
TagIdType TagManager::m_getTagId(std::string_view name, const TagsInfo<TagType> &info) const {
    for (const auto &tag : info.m_Tags) {
        if (name == tag.getName()) return tag.getId();
    }
    return nullTagId;
}

template<isTagType TagType>
const TagType &TagManager::m_getTag(TagIdType id, const TagsInfo<TagType> &info) const {
    if (m_checkIfValidId(id, info)) return info.m_Tags.at(id);
    else return info.m_Tags.at(nullTagId);
}

template<isTagType TagType>
void TagManager::m_readInfo(std::ifstream &in, TagsInfo<TagType> &info) {
    in.read(reinterpret_cast<char *>(&info.m_curSumOfTags), sizeof(info.m_curSumOfTags));
    in.read(reinterpret_cast<char *>(&info.m_curMaxTag), sizeof(info.m_curMaxTag));
    std::size_t size;
    in.read(reinterpret_cast<char *>(&size), sizeof(size));
    info.m_Tags.resize(size);
    // for (decltype(size) i = 0; i < size; ++i) {
    //     info.m_Tags[i].read();
    // }
    for (auto &tag : info.m_Tags) {
        tag.read(in);
    }
    in.read(reinterpret_cast<char *>(&size), sizeof(size));
    TagIdType tmp;
    for (decltype(size) i = 0; i < size; ++i) {
        in.read(reinterpret_cast<char *>(&tmp), sizeof(tmp));
        info.m_erasedTags.push(tmp);
    }
}

template<isTagType TagType>
void TagManager::m_writeInfo(std::ofstream &out, const TagsInfo<TagType> &info) const {
    out.write(reinterpret_cast<const char *>(&info.m_curSumOfTags), sizeof(info.m_curSumOfTags));
    out.write(reinterpret_cast<const char *>(&info.m_curMaxTag), sizeof(info.m_curMaxTag));
    auto size = info.m_Tags.size();
    out.write(reinterpret_cast<const char *>(&size), sizeof(size));
    for (const auto &tag : info.m_Tags) {
        tag.write(out);
    }
    auto tmp_heap = info.m_erasedTags;
    while (!info.m_erasedTags.empty()) {
        auto tmp = tmp_heap.top(); tmp_heap.pop();
        out.write(reinterpret_cast<const char *>(&tmp), sizeof(tmp));
    }
}

template<isTagType TagType>
TagIdType TagManager::m_getNewId(TagsInfo<TagType> &info) {
    if (!info.m_erasedTags.empty()) {
        ++info.m_curSumOfTags;
        auto id = info.m_erasedTags.top(); info.m_erasedTags.pop();
        return id;
    } else if (info.m_curMaxTag < maxTagId) {
        ++info.m_curSumOfTags;
        return ++info.m_curMaxTag;
    } else {
        return nullTagId;
    }
}

template<isTagType TagType>
bool TagManager::m_eraseTag(TagIdType id, TagsInfo<TagType> &info) {
    if (!m_checkIfValidId(id, info)) return false;
    --info.m_curSumOfTags;
    auto &tag = info.m_Tags[id];
    info.m_erasedTags.emplace(tag.getId());
    tag.m_id = nullTagId;
    return true;
}

template<isTagType TagType>
std::size_t TagManager::m_getSumOfTags(const TagsInfo<TagType> &info) const {
    return info.m_curMaxTag - info.m_erasedTags.size();
}
/* ====== END ====== */