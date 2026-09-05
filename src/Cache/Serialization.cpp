module Lattice.Cache.Serialization;

using namespace Lattice::Cache::Serialization;

auto Cache::ISection::GetName() const -> std::string {
    return m_sectionName;
}

auto Cache::ISection::SetName(const std::string &sectionName) -> void {
    m_sectionName = sectionName;
}

auto Cache::Section::IsArray() const -> bool {
    return false;
}

auto Cache::Section::operator[](const std::string &key) const -> std::string {
    return m_contents.at(key);
}

auto Cache::Section::operator[](const std::string &key) -> std::string& {
    return m_contents[key];
}

auto Cache::Section::Get(const std::string &key) const -> std::string {
    return m_contents.at(key);
}

auto Cache::Section::GetAll() const -> std::map<std::string, std::string> {
    return m_contents;
}

auto Cache::Section::Set(const std::string &key, const std::string &value) -> std::shared_ptr<Section> {
    m_contents[key] = value;

    return shared_from_this();
}

auto Cache::SectionArray::IsArray() const -> bool {
    return true;
}

auto Cache::SectionArray::operator[](int index) const -> const std::shared_ptr<Section> {
    return m_arraySections.at(index);
}

auto Cache::SectionArray::operator[](int index) -> std::shared_ptr<Section> {
    return m_arraySections[index];
}

auto Cache::SectionArray::Get(int index) const -> const std::shared_ptr<Section> {
    return m_arraySections.at(index);
}

auto Cache::SectionArray::GetAll() const -> const std::vector<std::shared_ptr<Section>>& {
    return m_arraySections;
}

auto Cache::SectionArray::GetAll() -> std::vector<std::shared_ptr<Section>>& {
    return m_arraySections;
}

auto Cache::SectionArray::Set(int index, const std::shared_ptr<Section> &section) -> std::shared_ptr<SectionArray> {
    if (index >= m_arraySections.size()) {
        m_arraySections.resize(index + 1);
    }

    m_arraySections[index] = section;

    return shared_from_this();
}

auto Cache::SectionArray::Add(const std::shared_ptr<Section> &section) -> std::shared_ptr<SectionArray> {
    m_arraySections.push_back(section);

    return shared_from_this();
}

auto Cache::SectionArray::SetSize(std::size_t size) -> std::shared_ptr<SectionArray> {
    m_arraySections.resize(size);

    return shared_from_this();
}

auto Cache::SectionArray::GetSize() const -> std::size_t {
    return m_arraySections.size();
}

auto Cache::operator[](const std::string &sectionName) const -> const std::shared_ptr<ISection> {
    return m_cacheSections.at(sectionName);
}

auto Cache::operator[](const std::string &sectionName) -> std::shared_ptr<ISection> {
    return m_cacheSections[sectionName];
}

auto Cache::ContainsSection(const std::string &sectionName) const -> bool {
    return m_cacheSections.contains(sectionName);
}

auto Cache::GetSection(const std::string &sectionName) const -> const std::shared_ptr<ISection> {
    return m_cacheSections.at(sectionName);
}

auto Cache::GetSection(const std::string &sectionName) -> std::shared_ptr<ISection> {
    return m_cacheSections[sectionName];
}
auto Cache::AddSection(const std::shared_ptr<ISection> &section) -> Cache& {
    m_cacheSections[section->GetName()] = section;
    return *this;
}

auto Cache::GetCacheName() const -> std::string {
    return m_cacheName;
}

auto Cache::SetCacheName(const std::string &cacheName) -> Cache& {
    m_cacheName = cacheName;
    
    return *this;
}

auto Cache::GetCacheLocation() const -> std::filesystem::path {
    return m_cachePath;
}

auto Cache::MarkDirty() -> Cache& {
    m_isDirty = true;

    return *this;
}

auto Cache::IsDirty() const -> bool {
    return m_isDirty;
}
